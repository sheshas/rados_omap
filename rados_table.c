#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <strings.h>
#include <assert.h>
#include <rados/librados.h>

#define ROW_PREFIX "row"
#define COL_PREFIX "field"

/* Passed in from the Makefile */
#ifdef DEBUG
#define DBG(_a) printf _a
#else
#define DBG(_a)
#endif

static void initialize(char *pool, rados_t *cluster, rados_ioctx_t *ioctx)
{
    DBG(("Create Cluster\n"));
    assert (rados_create(cluster, NULL) == 0);
    DBG(("Read Configuration file\n"));
    assert (rados_conf_read_file(*cluster, "/etc/ceph/ceph.conf") == 0);
    DBG(("Connecting ...\n"));
    assert (rados_connect(*cluster) == 0);
    DBG(("Create IO context\n"));
    assert (rados_ioctx_create(*cluster, pool, ioctx) == 0);
    DBG(("Environment initialized\n"));
}

static void usage(const char *pgm)
{
    printf("Usage: %s <options>\n", pgm);
    printf("\t-p : Pool Name\n");
    printf("\t-r : Number of Rows\n");
    printf("\t-c : Number of Columns\n");
    printf("\t-s : Data size\n");
}

int main(int argc, char **argv)
{
    char    *pool;
    int     nrows, ncols;
    int     reqd_args = 0;
    int     rid, cid;
    int     dsz = 0;

    while (1) {
        int c = getopt(argc, argv, "p:r:c:s:");
        if (c == -1)
            break;
        switch (c) {
        case 'p':
            pool = strdup(optarg);
            reqd_args++;
            break;
        case 'r':
            nrows = atoi(optarg);
            reqd_args++;
            break;
        case 'c':
            ncols = atoi(optarg);
            reqd_args++;
            break;
        case 's':
            dsz = atoi(optarg);
            break;
        default:
            usage(argv[0]);
            exit(1);
        }
    }

    if (reqd_args < 3) {
        usage(argv[0]);
        exit(1);
    }

    DBG(("conf: pool=%s, nrows=%d, ncols=%d, dsz: %d\n",
        pool, nrows, ncols, dsz));

    rados_t cluster = NULL;
    rados_ioctx_t ioctx = NULL;

  // connect to rados
    initialize(pool, &cluster, &ioctx);

    assert(cluster);
    assert(ioctx);

    char    *keys[1024];
    char    *vals[1024];
    size_t  sz[1024];
    char    keyname[1024];
    char    data[dsz];

    rados_write_op_t wrop = rados_create_write_op();
    assert(wrop);
    rados_write_op_assert_exists(wrop);
    rados_write_op_create(wrop, LIBRADOS_CREATE_EXCLUSIVE, NULL);
    memset (data, 'S', sizeof(data)); 

    for (rid = 0; rid < nrows; rid++) {
        char objname[1024];
        sprintf(objname, "%s%d", ROW_PREFIX, rid);
        rados_write_full(ioctx, objname, "", 0);

        for (cid = 0; cid < ncols; cid++) {
            sprintf(keyname, "%s.%s%d", objname,
                COL_PREFIX, cid);
            if (keys[cid])
                free(keys[cid]);
            if (vals[cid])
                free(vals[cid]);
            keys[cid] = strdup(keyname);
            vals[cid] = strdup(data);
            sz[cid] = sizeof(data);
            DBG(("Key: %s, sz: %d\n", keys[cid], (int) sz[cid]));
        }

        rados_write_op_omap_set(wrop, (char const * const *)keys,
            (char const * const *)vals, sz, ncols);
        rados_write_op_operate(wrop, ioctx, objname, NULL,
            LIBRADOS_OPERATION_NOFLAG);
    }

    rados_write_op_remove(wrop);
    rados_release_write_op(wrop);
    if (keys[cid])
        free(keys[cid]);
    if (vals[cid])
        free(vals[cid]);

    rados_ioctx_destroy(ioctx);
    rados_shutdown(cluster);
    free(pool);
    return 0;
}
