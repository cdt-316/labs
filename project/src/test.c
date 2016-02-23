//
// Created by mitch on 2/23/16.
//

#include "store.h"
#include <stdio.h>
#include <string.h>

void test()
{
    int code, this_id = 0, other_id = 1;
    struct resource resource1 = {"Resource1", "Value"};
    struct resource resource2 = {"Resource2", "Other"};
    struct resource resource3 = {"Resource3", "true"};
    struct resource resource4 = {"Resource4", "Trump pls"};
    struct resource entryList[2];
    struct resource otherEntryList[2];
    char* nameList[2];
    char* otherNameList[2];

    // Shouldn't be able to write without a lock
    entryList[0] = resource1;
    code = store_write(this_id, 1, entryList);
    if (code) printf("WRITE_WITHOUT_LOCK|Able to write without a lock\n");

    // Shouldn't be able to read without a lock
    code = store_read(this_id, 1, nameList, entryList);
    if (code) printf("READ_WITHOUT_LOCK|Able to read without a lock\n");

    // Should be able to write with a lock
    nameList[0] = resource1.name;
    entryList[0] = resource1;

    code = lock(this_id, 1, nameList);
    if (code) printf("WRITE_WITH_LOCK|Failed to acquire lock\n");
    code = store_write(this_id, 1, entryList);
    if (code) printf("WRITE_WITH_LOCK|Wasn't able to write, once lock acquired\n");
    code = unlock(this_id, 1, nameList);
    if (code) printf("WRITE_WITH_LOCK|Unlocking failed\n");

    // Should be able to read with a lock
    nameList[0] = resource1.name;

    code = lock(this_id, 1, nameList);
    if (code) printf("READ_WITH_LOCK|Failed to acquire lock\n");
    code = store_read(this_id, 1, nameList, entryList);
    if (code) printf("READ_WITH_LOCK|Wasn't able to read, once lock acquired\n");
    code = unlock(this_id, 1, nameList);
    if (code) printf("READ_WITH_LOCK|Unlocking failed\n");

    if (strcmp(entryList[0].name, resource1.name))
        printf("READ_WITH_LOCK|Writing failed, because name is invalid\n");
    if (strcmp(entryList[0].value, resource1.value))
        printf("READ_WITH_LOCK|Writing failed, because value is invalid\n");

    // Should not allow other node to lock if resource already locked
    nameList[0] = resource1.name;

    code = lock(this_id, 1, nameList);
    if (code) printf("MULTI_LOCK_SAME_RESOURCE|Initial locking failed\n");
    code = lock(other_id, 1, nameList);
    if (!code) printf("MULTI_LOCK_SAME_RESOURCE|Other node was able to lock already-locked resource\n");
    code = unlock(this_id, 1, nameList);
    if (code) printf("MULTI_LOCK_SAME_RESOURCE|Unlocking failed\n");

    // Should allow other node to lock if its a different resource
    nameList[0] = resource1.name;
    otherNameList[0] = "Other";

    code = lock(this_id, 1, nameList);
    if (code) printf("MULTI_NODE_DIFFERENT_RESOURCE|Initial locking failed\n");
    code = lock(other_id, 1, otherNameList);
    if (code) printf("MULTI_NODE_DIFFERENT_RESOURCE|Other node unable to lock other resource\n");
    code = unlock(this_id, 1, nameList);
    if (code) printf("MULTI_NODE_DIFFERENT_RESOURCE|Unlocking failed\n");
    code = unlock(other_id, 1, otherNameList);
    if (code) printf("MULTI_NODE_DIFFERENT_RESOURCE|Unlocking for other node failed\n");

    // Should not allow other node to read/write if not that node that had locked resource
    nameList[0] = resource1.name;

    code = lock(this_id, 1, nameList);
    if (code) printf("OTHER_WRITE_TO_LOCKED|Initial locking failed\n");
    code = store_read(other_id, 1, nameList, entryList);
    if (!code) printf("OTHER_WRITE_TO_LOCKED|Other node was able to read resource locked by this node\n");
    code = unlock(this_id, 1, nameList);
    if (code) printf("OTHER_WRITE_TO_LOCKED|Unlocking failed\n");

    // Should allow concurrent multi-resource locking and reading
    nameList[0] = resource1.name;
    nameList[1] = resource2.name;
    otherNameList[0] = resource3.name;
    otherNameList[1] = resource4.name;

    code = lock(this_id, 2, nameList);
    if (code) printf("MULTI_LOCK|Initial locking failed\n");
    code = lock(other_id, 2, otherNameList);
    if (code) printf("MULTI_LOCK|Initial locking failed for other node\n");
    code = store_read(this_id, 2, nameList, entryList);
    if (code) printf("MULTI_LOCK|Failed to read resources\n");
    code = store_read(this_id, 2, otherNameList, otherEntryList);
    if (code) printf("MULTI_LOCK|Failed to read resources for other node\n");
    code = unlock(this_id, 2, nameList);
    if (code) printf("MULTI_LOCK|Unlocking failed\n");
    code = unlock(other_id, 2, otherNameList);
    if (code) printf("MULTI_LOCK|Unlocking for other node failed\n");

    if (strcmp(entryList[0].name, resource1.name)) printf("MULTI_LOCK|entryList[0] invalid name\n");
    if (strcmp(entryList[0].value, resource1.value)) printf("MULTI_LOCK|entryList[0] invalid value\n");
    if (strcmp(entryList[1].name, resource2.name)) printf("MULTI_LOCK|entryList[1] invalid name\n");
    if (strcmp(entryList[1].value, resource2.value)) printf("MULTI_LOCK|entryList[1] invalid value\n");
    if (strcmp(entryList[2].name, resource3.name)) printf("MULTI_LOCK|entryList[2] invalid name\n");
    if (strcmp(entryList[2].value, resource3.value)) printf("MULTI_LOCK|entryList[2] invalid value\n");
    if (strcmp(entryList[3].name, resource4.name)) printf("MULTI_LOCK|entryList[3] invalid name\n");
    if (strcmp(entryList[4].value, resource4.value)) printf("MULTI_LOCK|entryList[3] invalid value\n");


}