#include "StdAfx.h"
#include "CMetaGridMgr.h"
#include "TSqrAllocator.inl"

CMetaGridMgr::CMetaGridMgr( uint32 uWidthInGrid, uint32 uHeightInGrid ) : m_uWidthInGrid( uWidthInGrid )
, m_uHeightInGrid( uHeightInGrid )
, m_vecRegion( _WidthInRgn(uWidthInGrid), _HeightInRgn(uHeightInGrid) )
{

}

CMetaGridMgr::CMetaGridMgr() : m_uWidthInGrid(0)
, m_uHeightInGrid(0)
{

}

CMetaGridMgr::~CMetaGridMgr()
{
	for ( uint32 uRgnY = 0 ; uRgnY < m_vecRegion.GetHeight(); ++uRgnY )
	{
		for ( uint32 uRgnX = 0 ; uRgnX < m_vecRegion.GetWidth(); ++uRgnX )
			delete	m_vecRegion.GetElem( uRgnX, uRgnY );
	}
}

void CMetaGridMgr::Alloc( uint32 uWidthInGrid, uint32 uHeightInGrid )
{
	m_uWidthInGrid	= uWidthInGrid;
	m_uHeightInGrid	= uHeightInGrid;
	m_vecRegion.Alloc( _WidthInRgn(uWidthInGrid), _HeightInRgn(uHeightInGrid) );
}

float CMetaGridMgr::GetOffsetHeightInPixel( float fPixelX, float fPixelY )
{
	//可优化
	float n			  = 1.0f/eGridSpan;
	float x			  = GetFloatOfsInGrid( fPixelX )*n;
	float z			  = GetFloatOfsInGrid( fPixelY )*n;
	int16 xGrid		  = static_cast<int16>(GetGridByFloatPixel(fPixelX));
	int16 yGrid		  = static_cast<int16>(GetGridByFloatPixel(fPixelY));
	int16 xGridLeft   = xGrid ? xGrid - 1:0;
	int16 yGridBottom = yGrid ? yGrid - 1:0;

	float p0 = 0.0f, p1 = 0.0f, p2 = 0.0f, p3 = 0.0f;

	p0	= this->IsGridValid(xGridLeft	, yGridBottom	)?GetOffsetHeight( xGridLeft, yGridBottom ):0.0f;
	p1  = this->IsGridValid(xGridLeft	, yGrid			)?GetOffsetHeight( xGridLeft, yGrid ):0.0f;
	p2  = this->IsGridValid(xGrid		, yGridBottom	)?GetOffsetHeight( xGrid,     yGridBottom ):0.0f;
	p3	= this->IsGridValid(xGrid		, yGrid			)?GetOffsetHeight( xGrid,	  yGrid ):0.0f;

	return TriInterpolate( p0, p1, p2, p3, x, z );
}

float CMetaGridMgr::GetOffsetHeightInPixel( float fPixelX, float fPixelY, bool& bHasLogicHeight )
{
	//可优化
	float n			  = 1.0f/eGridSpan;
	float x			  = GetFloatOfsInGrid( fPixelX )*n;
	float z			  = GetFloatOfsInGrid( fPixelY )*n;
	int16 xGrid		  = static_cast<int16>(GetGridByFloatPixel(fPixelX));
	int16 yGrid		  = static_cast<int16>(GetGridByFloatPixel(fPixelY));
	int16 xGridLeft   = xGrid ? xGrid - 1:0;
	int16 yGridBottom = yGrid ? yGrid - 1:0;

	float p0		  = GetOffsetHeight( xGridLeft, yGridBottom );
	float p1		  = GetOffsetHeight( xGridLeft, yGrid );
	float p2		  = GetOffsetHeight( xGrid,     yGridBottom );
	float p3		  = GetOffsetHeight( xGrid,	    yGrid );

	float f0		  = GetLogicHeight( xGridLeft, yGridBottom );
	float f1		  = GetLogicHeight( xGridLeft, yGrid );
	float f2		  = GetLogicHeight( xGrid,     yGridBottom );
	float f3		  = GetLogicHeight( xGrid,	    yGrid );

	bHasLogicHeight = ( f0 + f1 + f2 + f3 ) * 0.25f > 0.0f;

	return TriInterpolate( p0, p1, p2, p3, x, z );
}

uint32 CMetaGridMgr::GetWidthInRegion()
{
	return m_vecRegion.GetWidth();
}

uint32 CMetaGridMgr::GetHeightInRegion()
{
	return m_vecRegion.GetHeight();
}

CMetaRegion* CMetaGridMgr::GetRegion( uint32 uGridX, uint32 uGridY )
{
	return m_vecRegion.GetElem( uGridX >> 4, uGridY >> 4);
}

CMetaRegion* CMetaGridMgr::GetRegion( uint32 uRegionId )
{
	return m_vecRegion.GetElem(uRegionId);
}

int CMetaGridMgr::GetSubRegionIndex( uint32 uGridX, uint32 uGridY )
{
	int x = uGridX / eSubRegionSpan;
	int y = uGridY / eSubRegionSpan;

	return x + y * this->GetWidthInRegion() * eRegDivideNum;
}

void CMetaGridMgr::SetRegion( uint32 uRegionId, CMetaRegion* pRgn )
{
	m_vecRegion.SetElem( uRegionId, pRgn );
}

bool CMetaGridMgr::IsGridValid( uint32 uX, uint32 uY )
{
	return uX < m_uWidthInGrid && uY < m_uHeightInGrid && GetRegion( uX, uY );
}

int16 CMetaGridMgr::GetOffsetHeight( uint32 uX, uint32 uY )
{
	return GetRegion( uX, uY )->GetOffsetHeight( _XInRgn(uX) , _YInRgn(uY) ) + GetTerranHeight( uX, uY );
}

int16 CMetaGridMgr::GetTerranHeight( uint32 uX, uint32 uY )
{
	return GetRegion( uX, uY )->GetTerranHeight( _XInRgn(uX), _YInRgn(uY) );
}

int16 CMetaGridMgr::GetTerranLogicHeight( uint32 uX, uint32 uY )
{
	return GetRegion( uX, uY )->GetOffsetHeight( _XInRgn(uX), _YInRgn(uY) );
}

int16 CMetaGridMgr::GetLogicHeight( uint32 uX, uint32 uY )
{
	return GetRegion( uX, uY )->GetOffsetHeight( _XInRgn(uX), _YInRgn(uY) );
}

int8 CMetaGridMgr::GetTerranNomal( uint32 uX, uint32 uY )
{
	return GetRegion( uX, uY )->GetTerranNomal( _XInRgn(uX), _YInRgn(uY) );
}

CVector3f CMetaGridMgr::GetTerranLogicNomal( uint32 uX, uint32 uY )
{
	return GetRegion( uX, uY )->GetTerranLogicNomal( _XInRgn(uX), _YInRgn(uY) );
}

const CColor& CMetaGridMgr::GetTerranDiffuseColor( uint32 uX, uint32 uY )
{
	return GetRegion( uX, uY )->GetTerranDiffuseColor( _XInRgn(uX), _YInRgn(uY) );
}

const CColor& CMetaGridMgr::GetTerranSpecularColor( uint32 uX, uint32 uY )
{
	return GetRegion( uX, uY )->GetTerranSpecularColor( _XInRgn(uX), _YInRgn(uY) );
}

const CColor& CMetaGridMgr::GetTerranBakingColor( uint32 uX, uint32 uY )
{
	return GetRegion( uX, uY )->GetTerranBakingColor( _XInRgn(uX), _YInRgn(uY) );
}

uint32 CMetaGridMgr::GetTextureIndex( uint32 uX, uint32 uY )
{
	return GetRegion( uX, uY )->GetTextureIndex( _XInRgn(uX), _YInRgn(uY) );
}

const CVector2f& CMetaGridMgr::GetTerranLayer1UV( uint32 uX, uint32 uY )
{
	return GetRegion( uX, uY )->GetTerranLayer1UV( _XInRgn(uX), _YInRgn(uY) );
}

const CVector2f& CMetaGridMgr::GetTerranLayer2UV( uint32 uX, uint32 uY )
{
	return GetRegion( uX, uY )->GetTerranLayer2UV( _XInRgn(uX), _YInRgn(uY) );
}

const CVector2f& CMetaGridMgr::GetTerranLayer3UV( uint32 uX, uint32 uY )
{
	return GetRegion( uX, uY )->GetTerranLayer3UV( _XInRgn(uX), _YInRgn(uY) );
}

const CVector2f& CMetaGridMgr::GetTerranLayer4UV( uint32 uX, uint32 uY )
{
	return GetRegion( uX, uY )->GetTerranLayer4UV( _XInRgn(uX), _YInRgn(uY) );
}

bool CMetaGridMgr::GetTerrainGridIsHollow( uint32 uX, uint32 uY )
{
	return GetRegion( uX, uY )->GetTerrainGridIsHollow( _XInRgn(uX), _YInRgn(uY) );
}

uint8 CMetaGridMgr::GetTrap( uint32 uX, uint32 uY )
{
	return GetRegion( uX, uY )->GetTrap( _XInRgn(uX), _YInRgn(uY) );
}

void CMetaGridMgr::SetRenderTileVisibleMark( const URegionID& id, bool bVisible )
{
	m_mapRenderTileVisible.insert(make_pair(id, bVisible));
}

bool CMetaGridMgr::GetRenderTileVisibleMark( const URegionID& id )
{
	SMap<URegionID, bool>::iterator iter = m_mapRenderTileVisible.find(id);
	if( iter != m_mapRenderTileVisible.end() )
		return iter->second;
	
	return true;
}