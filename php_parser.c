#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ph7.h"

char *php_compile(char *file_location)
{
    ph7 *pEngine;
    ph7_vm *pVm;
    int rc;

    rc = ph7_init(&pEngine);
    if (rc != PH7_OK)
    {
        perror("Error while allocating a new PH7 engine instance");
        // exit(0);
    }

    rc = ph7_compile_file(pEngine, file_location, &pVm, 0);
    if (rc != PH7_OK)
    {
        perror("Compile error");
        // exit(0);
    }

    rc = ph7_vm_exec(pVm, 0);
    if (rc != PH7_OK)
    {
        perror("VM execution error");
        // exit(0);
    }

    void *pOutput;
    unsigned int nLen;

    ph7_vm_config(pVm, PH7_VM_CONFIG_EXTRACT_OUTPUT, &pOutput, &nLen);
    if (nLen > 0)
    {
        return (char *)pOutput;
    }
    else
    {
        return "500";
    }
}

char *php_post_compile(char *file_location, char *request)
{
}