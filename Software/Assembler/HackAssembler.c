#include <Hacklib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(p) _mkdir(p)
#define HACK_DIR "..\\.HACK"
#define HEX_DIR "..\\.HEX"
#define HACK_FMT "..\\.HACK\\%s.hack"
#define HEX_FMT "..\\.HEX\\%s.hex"
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(p) mkdir((p), 0777)
#define HACK_DIR "../.HACK"
#define HEX_DIR "../.HEX"
#define HACK_FMT "../.HACK/%s.hack"
#define HEX_FMT "../.HEX/%s.hex"
#endif

static int write_all(const char *path, const char *buf, size_t n)
{
    FILE *fp = fopen(path, "w");
    if (!fp)
    {
        perror("fopen");
        return -1;
    }
    if (n && fwrite(buf, 1, n, fp) != n)
    {
        perror("fwrite");
        fclose(fp);
        return -1;
    }
    fclose(fp);
    return 0;
}

static int options(int argc, const char *argv[])
{
    if (MKDIR(HACK_DIR) == -1 && errno != EEXIST)
    {
        perror("mkdir HACK");
        return 1;
    }
    if (MKDIR(HEX_DIR) == -1 && errno != EEXIST)
    {
        perror("mkdir HEX");
        return 1;
    }

    if (argc != 4)
    {
        fprintf(stderr,
                "usage:\n"
                ".\\bin\\HackAssembler <inputpath> ([-o <name>] | [-b <name>] | [-h <name>])\n"
                "  -o <name> : write ..\\.HACK\\<name>.hack\n"
                "  -h <name> : write ..\\.HEX\\<name>.hex\n"
                "  -b <name> : write both ..\\.HACK\\<name>.hack and ..\\.HEX\\<name>.hex\n");
        return 1;
    }

    const char *in = argv[1];
    const char *opt = argv[2];
    const char *out = argv[3];

    String bin = hackAssembler(in);
    if (!bin.str)
    {
        fprintf(stderr, "hackAssembler failed...\n");
        return 1;
    }

    // Finale step
    FILE *fp = fopen("..\\.AssemblerSteps\\4-out.hack", "w");
    if (!fp)
    {
        fprintf(stderr, "hackAssembler failed\n");
        return 1;
    }
    fwrite(bin.str, 1, bin.size - 1, fp);
    fclose(fp);

    size_t bin_bytes = (bin.size ? bin.size - 1 : 0);

    int rc = 0;

    if (strcmp(opt, "-o") == 0)
    {
        int need = snprintf(NULL, 0, HACK_FMT, out);
        char *p_hack = (char *)malloc((size_t)need + 1);
        if (!p_hack)
        {
            perror("malloc");
            free(bin.str);
            return 1;
        }
        snprintf(p_hack, (size_t)need + 1, HACK_FMT, out);

        rc = write_all(p_hack, bin.str, bin_bytes);
        free(p_hack);
    }
    else if (strcmp(opt, "-h") == 0)
    {
        String hx = convToHex(bin);
        if (!hx.str)
        {
            fprintf(stderr, "convToHex failed\n");
            free(bin.str);
            return 1;
        }
        size_t hx_bytes = (hx.size ? hx.size - 1 : 0);

        int need = snprintf(NULL, 0, HEX_FMT, out);
        char *p_hex = (char *)malloc((size_t)need + 1);
        if (!p_hex)
        {
            perror("malloc");
            free(hx.str);
            free(bin.str);
            return 1;
        }
        snprintf(p_hex, (size_t)need + 1, HEX_FMT, out);

        rc = write_all(p_hex, hx.str, hx_bytes);
        free(p_hex);
        free(hx.str);
    }
    else if (strcmp(opt, "-b") == 0)
    {
        String hx = convToHex(bin);
        if (!hx.str)
        {
            fprintf(stderr, "convToHex failed\n");
            free(bin.str);
            return 1;
        }
        size_t hx_bytes = (hx.size ? hx.size - 1 : 0);

        int need_h = snprintf(NULL, 0, HACK_FMT, out);
        int need_x = snprintf(NULL, 0, HEX_FMT, out);
        char *p_hack = (char *)malloc((size_t)need_h + 1);
        char *p_hex = (char *)malloc((size_t)need_x + 1);
        if (!p_hack || !p_hex)
        {
            perror("malloc");
            free(p_hack);
            free(p_hex);
            free(hx.str);
            free(bin.str);
            return 1;
        }

        snprintf(p_hack, (size_t)need_h + 1, HACK_FMT, out);
        snprintf(p_hex, (size_t)need_x + 1, HEX_FMT, out);

        rc = write_all(p_hack, bin.str, bin_bytes);
        if (rc == 0)
            rc = write_all(p_hex, hx.str, hx_bytes);

        free(p_hack);
        free(p_hex);
        free(hx.str);
    }
    else
    {
        fprintf(stderr, "unknown option: %s\n", opt);
        rc = 1;
    }

    free(bin.str);
    return rc;
}

int main(int argc, const char *argv[])
{
    return options(argc, argv);
}
