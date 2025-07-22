#include <stdio.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "cmds.pb.h"
#include "cmds_parser.h"

int main()
{
    /* This is the buffer where we will store our message. */
    uint8_t buffer[128];
    size_t req_length = 0;
    size_t res_length = 0;
    bool status;

    // Client SEND
    request_hndlr_t *cmd_handlers = request_handlers_get();

    /* Encode our message */
    {
        request req = request_init_zero;
        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        req.which_pl = request_version_get_tag;
        status = pb_encode(&stream, request_fields, &req);
        req_length = stream.bytes_written;
        if (!status)
        {
            printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
            return 1;
        }
    }
    // SERVER RECEIVE
    {
        /* Allocate space for the decoded message. */
        request req = request_init_zero;
        response res = response_init_zero;

        pb_istream_t i_stream = pb_istream_from_buffer(buffer, req_length);
        status = pb_decode(&i_stream, request_fields, &req);
        /* Check for errors... */
        if (!status)
        {
            printf("Decoding failed: %s\n", PB_GET_ERROR(&i_stream));
            return 1;
        }

        cmd_handlers[req.which_pl](&req, &res);

        pb_ostream_t o_stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        status = pb_encode(&o_stream, response_fields, &res);
        res_length = o_stream.bytes_written;
        if (!status)
        {
            printf("Encoding failed: %s\n", PB_GET_ERROR(&o_stream));
            return 1;
        }
    }
    // SERVER SEND

    // CLIENT RECEIVE
    {
        response res = response_init_zero;
        pb_istream_t stream = pb_istream_from_buffer(buffer, res_length);
        status = pb_decode(&stream, response_fields, &res);
        /* Check for errors... */
        if (!status)
        {
            printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
            return 1;
        }

        if (res.hdr.ret != ret_code_OK)
        {
            printf("Command failed! (%d - %s)\n", res.hdr.ret, res.hdr.err_msg);
        }
        else
        {
            printf("%d, %s, %s, %s", res.pl.version_get.version, res.pl.version_get.branch, res.pl.version_get.sha1,
                res.pl.version_get.commit_date);
        }

    }

    return 0;
}