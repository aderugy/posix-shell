#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "lexer/splitter.h"
#include "streams/streams.h"

static int shard_cmp(struct shard *expected, struct shard *actual)
{
    if (!expected || !actual)
    {
        return expected == actual;
    }

    return strcmp(expected->data, actual->data) == 0
        && expected->quoted == actual->quoted;
}

static void test_splitter(char *path, struct shard expected[], size_t size)
{
    printf("Test: %s\n", path);

    struct stream *stream = stream_from_file(path);
    if (!stream)
    {
        perror(NULL);
        errx(1, "Failed to create stream");
    }

    for (size_t i = 0; i < size; i++)
    {
        struct shard *actual = splitter_next(stream);

        if (!shard_cmp(&(expected[i]), actual))
        {
            printf("Failed:\n");
            printf("\texpected: %s (%d)\n", expected[i].data,
                   expected[i].quoted);

            if (!actual)
            {
                printf("\tgot: (nil)\n");
            }
            else
            {
                printf("\tactual: %s (%d)\n", actual->data, actual->quoted);
            }

            shard_free(actual);
            return;
        }

        shard_free(actual);
    }

    stream_close(stream);
    printf("Success\n");
}

int main(void)
{
    char *path;
    struct shard shards1[] = {
        { 0, "if" },          { 0, "echo" }, { 0, "ok" },     { 0, ";" },
        { 0, "then" },        { 0, "echo" }, { 0, "foobar" }, { 0, ">" },
        { 0, "example.txt" }, { 0, ";" },    { 0, "fi" }
    };

    path = "./tests/data/splitter1.sh";

    test_splitter(path, shards1, sizeof(shards1) / sizeof(struct shard));
    return 0;
}
