#pragma once
#include "CCoder.h"

namespace sqr
{
	class CFrameAnimTexCoder
		: public CCoder
		, public CCoderHelper
	{
		DECLARE_VERSION_MAP()
	public:
		CFrameAnimTexCoder();

		FourCC	GetChunkName(void);

		uint16	GetNewVer(void);

		uint16	GetChunkType();

		bool	Encode( WriteDataInf& wdi, OBJ_HANDLE handle );

		bool	DeCode1( const CDataChunk& inData, OBJ_HANDLE handle );

	private:
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle );
	};

}
