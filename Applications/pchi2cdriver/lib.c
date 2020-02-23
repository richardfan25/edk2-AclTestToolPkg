#include <Library/BaseLib.h>
//#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

#include "pchi2cdriver.h"



#define CPOOL_HEAD_SIGNATURE   SIGNATURE_32('C','p','h','d')

/** The UEFI functions do not provide a way to determine the size of an
    allocated region of memory given just a pointer to the start of that
    region.  Since this is required for the implementation of realloc,
    the memory head structure, CPOOL_HEAD, containing the necessary
    information is prepended to the requested space.

    The order of members is important.  This structure is 8-byte aligned,
    as per the UEFI specification for memory allocation functions.  By
    specifying Size as a 64-bit value and placing it immediately before
    Data, it ensures that Data will always be 8-byte aligned.

    On IA32 systems, this structure is 24 bytes long, excluding Data.
    On X64  systems, this structure is 32 bytes long, excluding Data.
**/
typedef struct {
  LIST_ENTRY      List;
  UINT32          Signature;
  UINT64          Size;
  CHAR8           Data[1];
} CPOOL_HEAD;

// List of memory allocated by malloc/calloc/etc.
static  LIST_ENTRY      MemPoolHead = INITIALIZE_LIST_HEAD_VARIABLE(MemPoolHead);

/** The malloc function allocates space for an object whose size is specified
    by size and whose value is indeterminate.

    This implementation uses the UEFI memory allocation boot services to get a
    region of memory that is 8-byte aligned and of the specified size.  The
    region is allocated with type EfiLoaderData.

    @param  size    Size, in bytes, of the region to allocate.

    @return   NULL is returned if the space could not be allocated and errno
              contains the cause.  Otherwise, a pointer to an 8-byte aligned
              region of the requested size is returned.<BR>
              If NULL is returned, errno may contain:
              - EINVAL: Requested Size is zero.
              - ENOMEM: Memory could not be allocated.
**/
void *malloc(UINTN Size)
{
  CPOOL_HEAD   *Head;
  void         *RetVal;
  EFI_STATUS    Status;
  UINTN         NodeSize;

  if( Size == 0) {
    //errno = EINVAL;   // Make errno diffenent, just in case of a lingering ENOMEM.
    DEBUG((DEBUG_ERROR, "ERROR malloc: Zero Size\n"));
    return NULL;
  }

  NodeSize = (UINTN)(Size + sizeof(CPOOL_HEAD));

  DEBUG((DEBUG_POOL, "malloc(%d): NodeSz: %d", Size, NodeSize));

  Status = gBS->AllocatePool( EfiLoaderData, NodeSize, (void**)&Head);
  if( Status != EFI_SUCCESS) {
    RetVal  = NULL;
    //errno   = ENOMEM;
    DEBUG((DEBUG_ERROR, "\nERROR malloc: AllocatePool returned %r\n", Status));
  }
  else {
    //assert(Head != NULL);
    // Fill out the pool header
    Head->Signature = CPOOL_HEAD_SIGNATURE;
    Head->Size      = NodeSize;

    // Add this node to the list
    (void)InsertTailList(&MemPoolHead, (LIST_ENTRY *)Head);

    // Return a pointer to the data
    RetVal          = (void*)Head->Data;
    DEBUG((DEBUG_POOL, " Head: %p, Returns %p\n", Head, RetVal));
  }

  return RetVal;
}

/** The free function causes the space pointed to by Ptr to be deallocated,
    that is, made available for further allocation.

    If Ptr is a null pointer, no action occurs.  Otherwise, if the argument
    does not match a pointer earlier returned by the calloc, malloc, or realloc
    function, or if the space has been deallocated by a call to free or
    realloc, the behavior is undefined.

    @param  Ptr     Pointer to a previously allocated region of memory to be freed.

**/
void free(void *Ptr)
{
  CPOOL_HEAD   *Head;

  Head = BASE_CR(Ptr, CPOOL_HEAD, Data);
  //assert(Head != NULL);
  DEBUG((DEBUG_POOL, "free(%p): Head: %p\n", Ptr, Head));

  if(Ptr != NULL) {
    if (Head->Signature == CPOOL_HEAD_SIGNATURE) {
      (void) RemoveEntryList((LIST_ENTRY *)Head);   // Remove this node from the malloc pool
      (void) gBS->FreePool (Head);                  // Now free the associated memory
    }
    else {
      //errno = EFAULT;
      DEBUG((DEBUG_ERROR, "ERROR free(0x%p): Signature is 0x%8X, expected 0x%8X\n",
             Ptr, Head->Signature, CPOOL_HEAD_SIGNATURE));
    }
  }
  DEBUG((DEBUG_POOL, "free Done\n"));
}

