-- Photon schema bootstrap (PostgreSQL 16.3+; forward-compatible with PG 18).

CREATE EXTENSION IF NOT EXISTS pgcrypto;
CREATE EXTENSION IF NOT EXISTS citext;

-- Keeps updated_at in sync on UPDATE (skips writes that don't touch updated_at).
CREATE OR REPLACE FUNCTION update_modified_column()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.updated_at IS NOT DISTINCT FROM OLD.updated_at THEN
        NEW.updated_at = NOW();
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- admins
CREATE TABLE admins (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    username CITEXT NOT NULL UNIQUE CHECK (char_length(username::text) BETWEEN 1 AND 64),
    password_hash TEXT NOT NULL CHECK (char_length(password_hash) >= 1),
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

CREATE TRIGGER update_admins_modtime
    BEFORE UPDATE ON admins
    FOR EACH ROW
    EXECUTE FUNCTION update_modified_column();

-- nodes
CREATE TABLE nodes (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    name TEXT NOT NULL CHECK (char_length(name) BETWEEN 1 AND 128),
    public_ipv4 INET NOT NULL,
    public_ipv6 INET,
    wg_ip INET NOT NULL UNIQUE,
    wg_pubkey TEXT UNIQUE CHECK (wg_pubkey IS NULL OR char_length(wg_pubkey) <= 64),
    is_active BOOLEAN NOT NULL DEFAULT TRUE,

    last_heartbeat_at TIMESTAMPTZ,
    health_status TEXT NOT NULL DEFAULT 'unknown'
        CHECK (health_status IN ('unknown', 'healthy', 'degraded', 'down')),
    health_message TEXT CHECK (health_message IS NULL OR char_length(health_message) <= 1024),

    max_users INTEGER CHECK (max_users IS NULL OR max_users >= 0),
    max_mbps INTEGER CHECK (max_mbps IS NULL OR max_mbps >= 0),
    max_sessions INTEGER CHECK (max_sessions IS NULL OR max_sessions >= 0),

    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

CREATE TRIGGER update_nodes_modtime
    BEFORE UPDATE ON nodes
    FOR EACH ROW
    EXECUTE FUNCTION update_modified_column();

CREATE INDEX idx_nodes_active ON nodes (is_active) WHERE is_active;

-- Xray inbounds per node
CREATE TABLE node_inbounds (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    node_id UUID NOT NULL REFERENCES nodes(id) ON DELETE CASCADE,
    tag TEXT NOT NULL CHECK (char_length(tag) BETWEEN 1 AND 64),
    protocol TEXT NOT NULL CHECK (char_length(protocol) BETWEEN 1 AND 32),
    port INTEGER NOT NULL CHECK (port > 0 AND port <= 65535),

    settings JSONB NOT NULL DEFAULT '{}'::jsonb,
    stream_settings JSONB NOT NULL DEFAULT '{}'::jsonb,

    config_version INTEGER NOT NULL DEFAULT 1 CHECK (config_version >= 1),
    last_applied_at TIMESTAMPTZ,

    is_active BOOLEAN NOT NULL DEFAULT TRUE,
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),

    UNIQUE (node_id, port),
    UNIQUE (node_id, tag)
);

CREATE TRIGGER update_node_inbounds_modtime
    BEFORE UPDATE ON node_inbounds
    FOR EACH ROW
    EXECUTE FUNCTION update_modified_column();

CREATE INDEX idx_inbounds_stream_settings ON node_inbounds USING GIN (stream_settings);
CREATE INDEX idx_inbounds_node_active ON node_inbounds (node_id) WHERE is_active;

-- end users (subscribers)
CREATE TABLE users (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    telegram_id BIGINT UNIQUE CHECK (telegram_id IS NULL OR telegram_id > 0),
    username TEXT CHECK (username IS NULL OR char_length(username) <= 128),

    uuid UUID NOT NULL UNIQUE DEFAULT gen_random_uuid(),

    sub_token TEXT NOT NULL UNIQUE DEFAULT encode(gen_random_bytes(24), 'hex')
        CHECK (char_length(sub_token) <= 64),

    data_limit_bytes BIGINT NOT NULL DEFAULT 53687091200 CHECK (data_limit_bytes >= 0), -- 50 gb default
    data_used_bytes BIGINT NOT NULL DEFAULT 0 CHECK (data_used_bytes >= 0),

    expire_at TIMESTAMPTZ,
    is_active BOOLEAN NOT NULL DEFAULT TRUE,

    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

CREATE TRIGGER update_users_modtime
    BEFORE UPDATE ON users
    FOR EACH ROW
    EXECUTE FUNCTION update_modified_column();

CREATE INDEX idx_users_active_expire ON users (expire_at) WHERE is_active;

-- traffic accounting
CREATE TABLE user_traffic_logs (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    user_id UUID NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    node_id UUID NOT NULL REFERENCES nodes(id) ON DELETE CASCADE,
    bytes_up BIGINT NOT NULL DEFAULT 0 CHECK (bytes_up >= 0),
    bytes_down BIGINT NOT NULL DEFAULT 0 CHECK (bytes_down >= 0),
    recorded_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

CREATE INDEX idx_traffic_user_time ON user_traffic_logs (user_id, recorded_at DESC);
CREATE INDEX idx_traffic_node_time ON user_traffic_logs (node_id, recorded_at DESC);
CREATE INDEX idx_traffic_time_brin ON user_traffic_logs USING BRIN (recorded_at);

-- billing / top-ups
CREATE TABLE transactions (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    user_id UUID NOT NULL REFERENCES users(id) ON DELETE CASCADE,

    amount_cents INTEGER NOT NULL,
    currency TEXT NOT NULL DEFAULT 'STARS' CHECK (char_length(currency) BETWEEN 1 AND 16),

    status TEXT NOT NULL DEFAULT 'pending' CHECK (
        status IN ('pending', 'completed', 'failed', 'refunded', 'cancelled')
    ),

    provider TEXT CHECK (provider IS NULL OR char_length(provider) <= 64),
    provider_txn_id TEXT CHECK (provider_txn_id IS NULL OR char_length(provider_txn_id) <= 255),
    idempotency_key TEXT CHECK (idempotency_key IS NULL OR char_length(idempotency_key) <= 128),

    description TEXT CHECK (description IS NULL OR char_length(description) <= 512),

    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    processed_at TIMESTAMPTZ
);

CREATE UNIQUE INDEX idx_transactions_provider_ext_id
    ON transactions (provider, provider_txn_id)
    WHERE provider IS NOT NULL AND provider_txn_id IS NOT NULL;

CREATE UNIQUE INDEX idx_transactions_idempotency
    ON transactions (idempotency_key)
    WHERE idempotency_key IS NOT NULL;

CREATE INDEX idx_transactions_user_created ON transactions (user_id, created_at DESC);

CREATE INDEX idx_transactions_user_completed_amount
    ON transactions (user_id) INCLUDE (amount_cents)
    WHERE status = 'completed';

CREATE VIEW user_balances AS
SELECT
    u.id AS user_id,
    COALESCE(
        SUM(t.amount_cents) FILTER (WHERE t.status = 'completed'),
        0
    )::bigint AS balance_cents
FROM users u
LEFT JOIN transactions t ON t.user_id = u.id
GROUP BY u.id;
