#include <aws/http/http.h>
#include <aws/http/request_response.h>
#include <stdio.h>
#include <string.h>

static int equals(struct aws_byte_cursor value, const char *expected) {
    return value.len == strlen(expected) && !memcmp(value.ptr, expected, value.len);
}

int main(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_byte_cursor value;
    struct aws_http_header header = {0};
    int result = 1;
    aws_http_library_init(allocator);
    struct aws_http_message *request = aws_http_message_new_request(allocator);
    if (!request) goto cleanup;
    header.name = aws_byte_cursor_from_c_str("Content-Type");
    header.value = aws_byte_cursor_from_c_str("application/octet-stream");
    if (aws_http_message_set_request_method(request, aws_byte_cursor_from_c_str("GET")) ||
        aws_http_message_set_request_path(request, aws_byte_cursor_from_c_str("/raster?band=1")) ||
        aws_http_message_add_header(request, header)) goto cleanup;
    if (aws_http_message_get_request_method(request, &value) || !equals(value, "GET")) goto cleanup;
    if (aws_http_message_get_request_path(request, &value) || !equals(value, "/raster?band=1")) goto cleanup;
    if (aws_http_message_get_header_count(request) != 1 ||
        aws_http_headers_get(aws_http_message_get_headers(request),
                             aws_byte_cursor_from_c_str("content-type"), &value) ||
        !equals(value, "application/octet-stream")) goto cleanup;
    puts("Installed HTTP request and case-insensitive header checks passed");
    result = 0;
cleanup:
    if (request) aws_http_message_release(request);
    aws_http_library_clean_up();
    return result;
}
