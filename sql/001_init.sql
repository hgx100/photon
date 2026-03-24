CREATE EXTENSION IF NOT EXISTS pgcrypto;

-- automatically updates the 'updated_at' timestamp on row modifications
CREATE OR REPLACE FUNCTION update_modified_column()
RETURNS TRIGGER AS $$
BEGIN
    NEW.updated_at = NOW();
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TABLE admins (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    username VARCHAR(64) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

CREATE TRIGGER update_admins_modtime
    BEFORE UPDATE ON admins FOR EACH ROW EXECUTE FUNCTION update_modified_column();

CREATE TABLE nodes (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    name VARCHAR(128) NOT NULL,
    public_ipv4 INET NOT NULL,
    public_ipv6 INET NOT NULL,
    wg_ip INET UNIQUE NOT NULL,
    wg_pubkey VARCHAR(64) UNIQUE,
    is_active BOOLEAN NOT NULL DEFAULT TRUE,
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

CREATE TRIGGER update_nodes_modtime
    BEFORE UPDATE ON nodes FOR EACH ROW EXECUTE FUNCTION update_modified_column();

-- xray configurations
CREATE TABLE node_inbounds (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    node_id UUID NOT NULL REFERENCES nodes(id) ON DELETE CASCADE,
    tag VARCHAR(64) NOT NULL, --xray inbound tag
    protocol VARCHAR(32) NOT NULL,
    port INTEGER NOT NULL CHECK (port > 0 AND port <= 65535),

    settings JSONB NOT NULL DEFAULT '{}'::jsonb,

    stream_settings JSONB NOT NULL DEFAULT '{}'::jsonb,

    is_active BOOLEAN NOT NULL DEFAULT TRUE,
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),

    -- prevent identical ports on the same node
    UNIQUE (node_id, port),
    -- prevent identical tags on the same node
    UNIQUE (node_id, tag)
);

CREATE TRIGGER update_node_inbounds_modtime
    BEFORE UPDATE ON node_inbounds FOR EACH ROW EXECUTE FUNCTION update_modified_column();

CREATE INDEX idx_inbounds_stream_settings ON node_inbounds USING GIN (stream_settings);

CREATE TABLE users (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    telegram_id BIGINT UNIQUE,
    username VARCHAR(128),

    uuid UUID UNIQUE NOT NULL DEFAULT gen_random_uuid(),

    sub_token VARCHAR(64) UNIQUE NOT NULL,

    data_limit_bytes BIGINT NOT NULL DEFAULT 53687091200, -- 50 gb default
    data_used_bytes BIGINT NOT NULL DEFAULT 0,

    expire_at TIMESTAMPTZ,
    is_active BOOLEAN NOT NULL DEFAULT TRUE,

    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

CREATE TRIGGER update_users_modtime
    BEFORE UPDATE ON users FOR EACH ROW EXECUTE FUNCTION update_modified_column();

CREATE TABLE user_traffic_logs (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    user_id UUID NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    node_id UUID NOT NULL REFERENCES nodes(id) ON DELETE CASCADE,
    bytes_up BIGINT NOT NULL DEFAULT 0,
    bytes_down BIGINT NOT NULL DEFAULT 0,
    recorded_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

CREATE INDEX idx_traffic_user_time ON user_traffic_logs (user_id, recorded_at);

CREATE TABLE transactions (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    user_id UUID NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    amount_cents INTEGER NOT NULL,      -- e.g., 100 = $1.00 (or 100 Telegram Stars)
    currency VARCHAR(16) NOT NULL DEFAULT 'STARS',
    description VARCHAR(255),
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);
