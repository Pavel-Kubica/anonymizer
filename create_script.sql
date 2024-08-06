SET allow_experimental_projection_optimization=1;

CREATE TABLE http_log
(
    `timestamp` DateTime,
    `resource_id` UInt64,
    `bytes_sent` UInt64,
    `request_time_milli` UInt64,
    `response_status` UInt16,
    `cache_status` LowCardinality(String),
    `method` LowCardinality(String),
    `remote_addr` String,
    `url` String,
    PROJECTION counts_by_cs
        (
        SELECT
            cache_status,
            count()
        GROUP BY cache_status
        ),
    PROJECTION counts_by_ra
        (
        SELECT
            remote_addr,
            count()
        GROUP BY remote_addr
        ),
    PROJECTION counts_by_ri
        (
        SELECT
            resource_id,
            count()
        GROUP BY resource_id
        ),
    PROJECTION counts_by_rs
        (
        SELECT
            response_status,
            count()
        GROUP BY response_status
        ),
    PROJECTION counts_by_ri_rs
        (
        SELECT
            resource_id,
            response_status,
            count()
        GROUP BY
            resource_id,
            response_status
        ),
    PROJECTION counts_by_ri_cs
        (
        SELECT
            resource_id,
            cache_status,
            count()
        GROUP BY
            resource_id,
            cache_status
        ),
    PROJECTION counts_by_ri_ra
        (
        SELECT
            resource_id,
            remote_addr,
            count()
        GROUP BY
            resource_id,
            remote_addr
        ),
    PROJECTION counts_by_rs_cs
        (
        SELECT
            response_status,
            cache_status,
            count()
        GROUP BY
            response_status,
            cache_status
        ),
    PROJECTION counts_by_rs_ra
        (
        SELECT
            response_status,
            remote_addr,
            count()
        GROUP BY
            response_status,
            remote_addr
        ),
    PROJECTION counts_by_cs_ra
        (
        SELECT
            cache_status,
            remote_addr,
            count()
        GROUP BY
            cache_status,
            remote_addr
        ),
    PROJECTION counts_by_rs_cs_ra
        (
        SELECT
            response_status,
            cache_status,
            remote_addr,
            count()
        GROUP BY
            response_status,
            cache_status,
            remote_addr
        ),
    PROJECTION counts_by_ri_rs_ra
        (
        SELECT
            resource_id,
            response_status,
            remote_addr,
            count()
        GROUP BY
            resource_id,
            response_status,
            remote_addr
        ),
    PROJECTION counts_by_ri_cs_ra
        (
        SELECT
            resource_id,
            cache_status,
            remote_addr,
            count()
        GROUP BY
            resource_id,
            cache_status,
            remote_addr
        ),
    PROJECTION counts_by_ri_rs_cs
        (
        SELECT
            resource_id,
            response_status,
            cache_status,
            count()
        GROUP BY
            resource_id,
            response_status,
            cache_status
        ),
    PROJECTION counts_by_ri_rs_cs_ra
        (
        SELECT
            resource_id,
            response_status,
            cache_status,
            remote_addr,
            count()
        GROUP BY
            resource_id,
            response_status,
            cache_status,
            remote_addr
        )
)
    ENGINE = MergeTree()
        ORDER BY (resource_id, response_status, cache_status, remote_addr)