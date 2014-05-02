/* Copyright Rebecca Skinner 2012
 * This source code is covered under the GNU Lesser General Public License.
 * See the LICENSE file for the complete text of the license covering this
 * file.
 */
#ifndef __LOCKFREE_QUEUE_H__
#define __LOCKFREE_QUEUE_H__

struct lf_queue;
struct queue;

typedef struct lf_queue lf_queue_t;
typedef struct queue queue_t;

struct queue
{
    queue_t* next;
    void*    value;
};

struct lf_queue
{
    queue_t* in;
    queue_t* swp;
    queue_t* out;
};

#endif
