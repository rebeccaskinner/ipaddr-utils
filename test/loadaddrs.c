/* Copyright Rebecca Skinner 2012
 * This source code is covered under the GNU Lesser General Public License.
 * See the LICENSE file for the complete text of the license covering this
 * file.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "addr_utils.h"
#include "iptree.h"

static void show_usage()
{
    printf("usage: loadaddrs <filename>\n");
}

int main(int argc, char** argv)
{
    ip_tree_t* tree;
    uint32_t*  addrs;
    uint32_t   num_addrs;
    uint32_t   found_addrs;

    if(2 != argc || !argv || !argv[1])
    {
        show_usage();
        return 1;
    }


    tree = load_addr_file(argv[1],&num_addrs);
    if(!num_addrs)
    {
        printf("load file error\n");
        return 1;
    }

    addrs = malloc(num_addrs * sizeof(uint32_t));
    found_addrs = iptree_get_sorted(tree,addrs);
    for(uint32_t i = 0; i < found_addrs; i++)
    {
        printf("%s\n",addr_to_string(addrs[i]));
    }

    return 0;

}
