//===============================================================================
// @ IvStackAllocator.h
// 
// Assertion macros
// ------------------------------------------------------------------------------
// Copyright (C) 2014 by James M. Van Verth. All rights reserved.
//
//===============================================================================

#ifndef __IvStackAllocator__h__
#define __IvStackAllocator__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvStackAllocator
{
public:
	IvStackAllocator(size_t totalBytes);
	~IvStackAllocator();

	size_t GetCurrentOffset() { return fCurrentOffset; }
	void Reset(size_t offset = 0) { fCurrentOffset = offset; }

	void* Allocate(size_t bytes);

	static IvStackAllocator* mScratchAllocator;

private:
	unsigned char* fAllocation;
	size_t         fTotalBytes;
	size_t         fCurrentOffset;
};


#endif
