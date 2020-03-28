/*
 * mytest_module
 */
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

static ngx_int_t ngx_http_mytest_handler(ngx_http_request_t *r);
static char * ngx_http_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

/*
 * --ngx_command_t：用于处理 nginx.conf 中的配置项结构
 */
static ngx_command_t ngx_http_mytest_commands[] = {
    {
        // 配置项名称
        ngx_string("mytest"),
        // 配置项可以出现的位置
        NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_HTTP_LMT_CONF | NGX_CONF_NOARGS,
        // 处理配置项参数的方法
        ngx_http_mytest,
        // 在配置文件中的偏移量
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },
    ngx_null_command
};

/*
 * --ngx_http_module_t 上下文结构体
 */
static ngx_http_module_t ngx_http_mytest_module_ctx = {
    NULL,
    NULL,

    NULL,
    NULL,

    NULL,
    NULL,

    NULL,
    NULL
};

/*
 * --ngx_module_t 
 *  configure 后，会被加入到 ngx_modules.c 中
 */
ngx_module_t ngx_http_mytest_module = {
    // ctx_index,index,spare0,spare1,spare2,spare3,version
    NGX_MODULE_V1,
    // 上下文结构体
    &ngx_http_mytest_module_ctx,
    // 用于处理 nginx.conf 中的配置项结构
    ngx_http_mytest_commands,
    // 模块类型，HTTP模块
    NGX_HTTP_MODULE,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NGX_MODULE_V1_PADDING
};

// 处理 nginx.conf 配置项参数的方法
static char * 
ngx_http_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf) 
{
    ngx_http_core_loc_conf_t *clcf;

    // 获取 mytest 配置项所属的配置块
    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);

    // 如果请求的主机域名、URI与mytest配置项所在的配置块相匹配，调用该方法
    clcf->handler = ngx_http_mytest_handler;

    return NGX_CONF_OK;
}

// 如果请求的主机域名、URI与mytest配置项所在的配置块相匹配，调用该方法
static ngx_int_t 
ngx_http_mytest_handler(ngx_http_request_t *r)
{
    // 必须是 GET 获得 HEAD 方法
    if (!(r->method & (NGX_HTTP_GET | NGX_HTTP_HEAD))) {
        return NGX_HTTP_NOT_ALLOWED;
    }

    // 丢弃请求中的包体
    ngx_int_t rc = ngx_http_discard_request_body(r);
    if (rc != NGX_OK) {
        return rc;
    }

    // Content-Type
    ngx_str_t type = ngx_string("text/plain");
    // 包体内容
    ngx_str_t response = ngx_string("Hello World!\ndevlepe a simple http module!\n");
    // 设置返回状态码
    r->headers_out.status = NGX_HTTP_OK;
    // 设置Content-Length
    r->headers_out.content_length_n = response.len;
    // 设置Content-Type
    r->headers_out.content_type = type;

    // 发送HTTP头部
    rc = ngx_http_send_header(r);
    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
        return rc;
    }

    // 构造包体
    ngx_buf_t *b;
    b = ngx_create_temp_buf(r->pool, response.len);
    if (b == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    // 将 Hello World 复制到 ngx_buf_t 指向的内存中
    ngx_memcpy(b->pos, response.data, response.len);
    b->last = b->pos + response.len;
    b->last_buf = 1;
    // 构造发送时的 ngx_chain_t 结构体
    ngx_chain_t out;
    out.buf = b;
    out.next = NULL;

    // 发送包体
    return ngx_http_output_filter(r, &out);
}
