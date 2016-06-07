#include "OdPlatformSettings.h"
#include "OdaCommon.h"

// TX routines.
#include "RxObjectImpl.h"
#include "RxDynamicModule.h"
#include "Gs/Gs.h"
#include "Gs/GsBaseVectorizer.h"

#include "ExSystemServices.h"
#include "ExHostAppServices.h"
#include "OdFileBuf.h"

// TD objects we need.
#include "DbDictionary.h"
#include "DbViewportTable.h"
#include "DbViewportTableRecord.h"

// Following headers correspond to each of six Teigha® Architecture modules.
#include "AECBase.h"
#include "AECArchBase.h"
#include "AECArchDACHBase.h"
#include "AECScheduleData.h"
#include "AECSchedule.h"
#include "AECStructureBase.h"

#include "Common/AECDisplayManager.h"
#include "DbObject/AECDbDispRepConfig.h"
#include "DbObject/AECDbDispRepSet.h"
#include "DispReps/AECDbDispRepArchBase.h"
#include "DispReps/AECDbDispRepArchDACHBase.h"
#include "DispReps/AECDbDispRepSchedule.h"
#include "DispReps/AECDbDispRepStructureBase.h"

#include "AECArchBaseDefs.h"
#include "DbAnchor/AECDbAnchorOpeningBaseToWall.h"
#include "DbEntity/AECDbDoor.h"
#include "DbEntity/AECDbOpening.h"
#include "DbEntity/AECDbSpace.h"
#include "DbEntity/AECDbWall.h"
#include "DbEntity/AECDbWindow.h"
#include "DbEntity/AECDb2dSection.h"
#include "DbEntity/AECDbBdgSectionLine.h"
#include "DbObject/AECDbCleanupGroupDef.h"
#include "DbObject/AECDbDoorStyle.h"
#include "DbObject/AECDbEndcapStyle.h"
#include "DbObject/AECDbOpeningEndcapStyle.h"
#include "DbObject/AECDbProfileDef.h"
#include "DbObject/AECDbSpaceStyle.h"
#include "DbObject/AECDbWallStyle.h"
#include "DbObject/AECDbWindowStyle.h"
#include "DbObject/AECDbSlabEdgeStyle.h"
#include "DbObject/AECDb2dSectionStyle.h"
#include "DbObject/AECDbDispRepSet.h"
#include "DispReps/AECDbDispRepArchBase.h"
#include "DispProps/AECDbDispPropsDoor.h"
#include "DispProps/AECDbDispPropsWallModel.h"
#include "DispProps/AECDbDispPropsWindow.h"
#include "Subobjects/AECAnchorToCurveX.h"
#include "Subobjects/AECAnchorToCurveY.h"
#include "Subobjects/AECAnchorToCurveZ.h"
#include "Subobjects/Ge/AECGeRing.h"
#include "Subobjects/Ge/AECGeProfile.h"
#include "Geometry/AECProfile2D.h"

#include "DbEntity/AECDbRoof.h"
#include "Geometry/AECProfile2D.h"
#include "Subobjects/Ge/AECGeRoofProfile.h"
#include "Subobjects/Ge/AECGeRoofSegment.h"
#include "Subobjects/Ge/AECGeRing.h"

#include "DbEntity/AECDbSlab.h"
#include "DbObject/AECDbSlabStyle.h"
#include "DispProps/AECDbDispPropsSlabPlan.h"
#include "DispReps/AECDbDispRepArchBase.h"
#include "Subobjects/Ge/AECGeSlabFace.h"
#include "Subobjects/Ge/AECGeRing.h"
#include "Subobjects/Slab/AECSlabStyleComp.h"

#define STL_USING_IOSTREAM
#include "OdaSTL.h"
#define  STD(a)  std:: a

#ifdef OD_HAVE_CONSOLE_H_FILE
#include <console.h>
#endif
#include "OdModuleNames.h"
#include "AECAppStaticModules.h"


/************************************************************************/
/* Define a module map for statically linked modules:                   */
/************************************************************************/
#ifndef _TOOLKIT_IN_DLL_

ODRX_DECLARE_ARCH_STATIC_MODULES_ENTRY_POINTS()

  ODRX_BEGIN_STATIC_MODULE_MAP()
  ODRX_DEFINE_ARCH_STATIC_APPMODULES()
  ODRX_END_STATIC_MODULE_MAP()

#endif

inline OdCmColor colorAt( OdUInt16 iIndex )
{ OdCmColor cColor; cColor.setColorIndex( iIndex ); return cColor; }


void init_display_system( OdDbDatabasePtr pDatabase )
{
  AECDisplayManager cDisplayManager( pDatabase );

  // Here we want to assign model representation of entities we use
  // for all types of orthographic view directions and default.
  // For this we to create the following:

  // Display Set with
  // model representations.
  OdDbObjectId idModel = AECDbDispRepSet::CreateAECObject( pDatabase, OD_T("Model Display Set") );
  AECDbDispRepSetPtr pDispSetM = idModel.openObject( OdDb::kForWrite );


  // Display Set with
  // plan representations.
  OdDbObjectId idPlan = AECDbDispRepSet::CreateAECObject( pDatabase, OD_T("Plan Display Set") );
  AECDbDispRepSetPtr pDispSetP = idPlan.openObject( OdDb::kForWrite );

  // Representations from AecBase DRX.
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepMvBlockRef::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepMvBlockRefModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepEntRef::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepMaskBlockRef::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepMassElemModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepPolygonModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepMassGroupModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepSlice::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepLayoutCurve::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepLayoutGrid2d::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepLayoutGrid3d::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepClipVolModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRep2dSection::desc() ) );

  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepMvBlockRef::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepMvBlockRefPlan100::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepEntRef::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepMaskBlockRefRcp::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepMassElemSchem::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepPolygonModel::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepMassGroupPlan::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepSlice::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepLayoutCurve::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepLayoutGrid2d::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepLayoutGrid3d::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepClipVolPlan::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRep2dSection::desc() ) );

  // Representations from AecArchBase DRX.
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepBdgElevLineModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepBdgSectionLineModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepBdgSectionModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepCeilingGridModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepCurtainWallLayoutModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepCurtainWallUnitModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepDoorModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepOpeningModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepRailingModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepRoofModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepRoofSlabModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepSlabModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepSpaceModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepStairModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepWallModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepWindowModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepWindowAssemblyModel::desc() ) );

  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepBdgElevLinePlan::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepBdgSectionLinePlan::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepBdgSectionSubDiv::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepCeilingGrid::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepCurtainWallLayoutPlan::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepCurtainWallUnitPlan::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepDoorPlan::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepOpening::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepRailingPlan::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepRoofPlan::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepRoofSlabPlan::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepSlabPlan::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepSpacePlan::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepStairPlan::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepWallPlan::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepWindowPlan::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepWindowAssemblyPlan::desc() ) );

  // Representations from AecArchDACHBase DRX.
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepDimGroupPlan::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepDimGroupPlan::desc() ) );

  // Representations from AecSchedule DRX.
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepScheduleTable::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepScheduleTable::desc() ) );

  // Representations from AecStructureBase DRX.
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECSDbDispRepMemberModelDesign::desc() ) );
  pDispSetP->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECSDbDispRepMemberPlanDesign::desc() ) );

  OdDbObjectId idDispCfg = AECDbDispRepConfig::GetStandardAECObject( pDatabase );
  AECDbDispRepConfigPtr pDispConf = idDispCfg.openObject( OdDb::kForWrite );

  pDispConf->SetAsDefault();

  pDispConf->SetIsViewportDependent( true );
  pDispConf->AssignDisplaySet( OdDb::kTopView,      pDispSetP->objectId() );
  pDispConf->AssignDisplaySet( OdDb::kBottomView,   pDispSetP->objectId() );

  pDispConf->AssignDisplaySet( OdDb::kNonOrthoView, pDispSetM->objectId() );
  pDispConf->AssignDisplaySet( OdDb::kFrontView,    pDispSetM->objectId() );
  pDispConf->AssignDisplaySet( OdDb::kBackView,     pDispSetM->objectId() );
  pDispConf->AssignDisplaySet( OdDb::kLeftView,     pDispSetM->objectId() );
  pDispConf->AssignDisplaySet( OdDb::kRightView,    pDispSetM->objectId() );

  pDispConf->SetFixedDisplaySet( pDispSetP->objectId() );
}

void init_display_system3D( OdDbDatabasePtr pDatabase )
{
  AECDisplayManager cDisplayManager( pDatabase );

  // Here we want to assign model representation of entities we use
  // for all types of orthographic view directions and default.
  // For this we to create the following:

  // Display Set with
  // model representations.
  OdDbObjectId idDispSet = AECDbDispRepSet::CreateAECObject( pDatabase, OD_T("Model Display Set") );
  AECDbDispRepSetPtr pDispSetM = idDispSet.openObject( OdDb::kForWrite );

  // Representations from AecBase DRX.
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepMvBlockRefModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepEntRef::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepMaskBlockRef::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepMassElemModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepPolygonModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepMassGroupModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepSlice::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepLayoutCurve::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepLayoutGrid2d::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepLayoutGrid3d::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepClipVolModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRep2dSection::desc() ) );

  // Representations from AecArchBase DRX.
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepBdgElevLineModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepBdgSectionLineModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepBdgSectionModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepCeilingGridModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepCurtainWallLayoutModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepCurtainWallUnitModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepDoorModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepOpeningModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepRailingModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepRoofModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepRoofSlabModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepSlabModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepSpaceModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepStairModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepWallModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepWindowModel::desc() ) );
  pDispSetM->AddDispRep( cDisplayManager.UpdateDisplayRepresentation( AECDbDispRepWindowAssemblyModel::desc() ) );

  OdDbObjectId idDispConf = AECDbDispRepConfig::GetStandardAECObject( pDatabase );
  AECDbDispRepConfigPtr pDispConf = idDispConf.openObject( OdDb::kForWrite );

  pDispConf->SetAsDefault();

  pDispConf->SetIsViewportDependent( false );

  pDispConf->AssignDisplaySet( OdDb::kNonOrthoView, pDispSetM->objectId() );
  pDispConf->AssignDisplaySet( OdDb::kTopView,      0 ); //pDispSetM->objectId() 
  pDispConf->AssignDisplaySet( OdDb::kBottomView,   0 ); //pDispSetM->objectId() 
  pDispConf->AssignDisplaySet( OdDb::kFrontView,    0 ); //pDispSetM->objectId() 
  pDispConf->AssignDisplaySet( OdDb::kBackView,     0 ); //pDispSetM->objectId() 
  pDispConf->AssignDisplaySet( OdDb::kLeftView,     0 ); //pDispSetM->objectId() 
  pDispConf->AssignDisplaySet( OdDb::kRightView,    0 ); //pDispSetM->objectId() 

  pDispConf->SetFixedDisplaySet( pDispSetM->objectId() );
}

void transform( const OdDbObjectId& idEntity, const OdGeMatrix3d& mTransform )
{
  OdDbEntityPtr pEntity = idEntity.openObject( OdDb::kForWrite );
  pEntity->transformBy( mTransform );
}

OdDbObjectId add_wall_style( OdDbDatabasePtr pDatabase )
{
  OdDbObjectId idResult =
    AECDbWallStyle::CreateAECObject( pDatabase, OD_T("Wall Style Created By Teigha(R) Architecture") );

  AECDbWallStylePtr pWallStyle =
    idResult.openObject( OdDb::kForWrite );

  pWallStyle->SetDescription( OD_T("Wall Style Description") );
  pWallStyle->SetDictRecordDescription( OD_T("Dialog caption") );

  pWallStyle->SetWallWidth( 4 );
  pWallStyle->SetWallWidthUsed( true );

  pWallStyle->SetBaseHeight( 110 );
  pWallStyle->SetBaseHeightUsed( true );

  pWallStyle->SetJustification( AECDefs::ewjLeft );
  pWallStyle->SetJustificationUsed( true );

  pWallStyle->SetAutomaticCleanups( true );
  pWallStyle->SetAutomaticCleanupsUsed( true );

  pWallStyle->SetCleanupRadius( 4 );
  pWallStyle->SetCleanupRadiusUsed( true );

  pWallStyle->SetFloorLineOffset( 3 );
  pWallStyle->SetFloorLineOffsetUsed( false );

  pWallStyle->SetRoofLineOffset( -3 );
  pWallStyle->SetRoofLineOffsetUsed( false );

  AECDisplayManager cDM( pDatabase );
  AECDbDispPropsWallModelPtr pOverrideModel =
    AECDbDispPropsWallModel::cast( pWallStyle->OverrideDispProps(
    cDM.UpdateDisplayRepresentation( AECDbDispRepWallModel::desc() ) ).openObject( OdDb::kForWrite ) );
  if ( !pOverrideModel.isNull() )
  {
    pOverrideModel->SetIsDisplayOpeningEndcaps( false );
    pOverrideModel->GetBoundaryCompByIndex( 0 )->SetColor( colorAt( 4 ) );
  }

  AECDbDispPropsWallPtr pOverridePlan =
    AECDbDispPropsWall::cast( pWallStyle->OverrideDispProps(
    cDM.UpdateDisplayRepresentation( AECDbDispRepWallPlan::desc() ) ).openObject( OdDb::kForWrite ) );
  if ( !pOverridePlan.isNull() )
  {
    pOverridePlan->GetBoundaryCompByIndex( 0 )->SetColor( colorAt( 4 ) );
  }

  return( pWallStyle->objectId() );
}

OdDbObjectId add_wall( OdDbDatabasePtr pDatabase, const OdDbObjectId& idStyle,
                      const OdGePoint2d& ptStart, const OdGePoint2d& ptEnd, double dBulge = 0 )
{
  AECDbWallPtr pWall =
    AECDbWall::CreateAECObject( pDatabase->getModelSpaceId(), idStyle );

  pWall->Set( ptStart, ptEnd, dBulge );
  pWall->SetDescription( OD_T("A Wall") );

  /*AECGe::Contour2D cContour;
  cContour.appendVertex( OdGePoint2d( 0,                      pWall->GetHeight()       ) );
  cContour.appendVertex( OdGePoint2d( pWall->GetLength() / 2, pWall->GetHeight() * 1.5 ) );
  cContour.appendVertex( OdGePoint2d( pWall->GetLength(),     pWall->GetHeight()       ) );

  AECWallCutLineSubPtr pTop = pWall->CreateTopCutLine();
  pTop->SetRawCutLine( cContour );*/

  return( pWall->objectId() );
}

OdDbObjectId add_window_style( OdDbDatabasePtr pDatabase )
{
  OdDbObjectId idWStyle =
    AECDbWindowStyle::CreateAECObject( pDatabase, OD_T("Window Style Created By Teigha(R) Architecture") );

  AECDbWindowStylePtr pWindowStyle = idWStyle.openObject( OdDb::kForWrite );

  pWindowStyle->SetDescription( OD_T("Window Style Description") );
  pWindowStyle->SetDictRecordDescription( OD_T("Dialog caption") );
  pWindowStyle->SetAutoAdjustToWidthOfWall( true );
  pWindowStyle->SetFrameWidth( 2 );
  pWindowStyle->SetFrameDepth( 5 );
  pWindowStyle->SetSashWidth( 2 );
  pWindowStyle->SetSashDepth( 3 );
  pWindowStyle->SetGlassThickness( 1 );
  pWindowStyle->SetWindowType( AECDefs::ewtGlider );
  pWindowStyle->SetWindowShape( AECDefs::esRectangular );

  AECDisplayManager cDM( pDatabase );
  AECDbDispPropsWindowPtr pOverrideModel =
    AECDbDispPropsWindow::cast( pWindowStyle->OverrideDispProps(
    cDM.UpdateDisplayRepresentation( AECDbDispRepWindowModel::desc() ) ).openObject( OdDb::kForWrite ) );
  if ( !pOverrideModel.isNull() )
  {
    pOverrideModel->GetFrameComp()->SetColor( colorAt( 1 ) );
    pOverrideModel->GetSashComp()->SetColor( colorAt( 2 ) );
    pOverrideModel->GetGlassComp()->SetColor( colorAt( 3 ) );
  }

  AECDbDispPropsWindowPtr pOverridePlan =
    AECDbDispPropsWindow::cast( pWindowStyle->OverrideDispProps(
    cDM.UpdateDisplayRepresentation( AECDbDispRepWindowPlan::desc() ) ).openObject( OdDb::kForWrite ) );
  if ( !pOverridePlan.isNull() )
  {
    pOverridePlan->GetFrameComp()->SetColor( colorAt( 1 ) );
    pOverridePlan->GetSashComp()->SetColor( colorAt( 2 ) );
    pOverridePlan->GetGlassComp()->SetColor( colorAt( 3 ) );
  }

  return( pWindowStyle->objectId() );
}

// Inserts a window into a database using the specified window style.
// If idWall parameter is not null it also attaches the window to the wall.
// Returns Object ID of newly created window.
OdDbObjectId add_window( OdDbDatabasePtr pDatabase, const OdDbObjectId& idStyle, const OdDbObjectId& idWall,
                        double dOffsetAlongX, double dOffsetAlongZ )
{
  AECDbWindowPtr pWindow = AECDbWindow::CreateAECObject( pDatabase->getModelSpaceId(), idStyle );

  pWindow->SetRise( 10 );
  pWindow->SetWidth( 40 );
  pWindow->SetHeight( 40 );

  pWindow->SetOpenPercent( 60 );
  pWindow->SetMeasureTo( AECDefs::eomtOutsideFrame );
  pWindow->SetLeaf( 10 );

  if ( !idWall.isNull() )
  {
    pWindow->AttachWallAnchor( idWall );

    AECDbAnchorEntToCurvePtr pAnchor = pWindow->GetAnchor().openObject( OdDb::kForWrite );
    pAnchor->GetXParams()->SetOffset( dOffsetAlongX );       
    pAnchor->GetZParams()->SetOffset( dOffsetAlongZ );
  }

  return( pWindow->objectId() );
}


// Inserts profile definition into a database.
// Returns Object ID of newly created profile definition.
OdDbObjectId add_profile_def( OdDbDatabasePtr pDatabase )
{
  OdDbObjectId idProfDef =
    AECDbProfileDef::CreateAECObject( pDatabase, OD_T("Profile Definition Created By Teigha(R) Architecture") );

  AECDbProfileDefPtr pProfileDefinition = idProfDef.openObject( OdDb::kForWrite );

  AECGe::Profile2D cProfile;
  cProfile.resize( 2 );

  cProfile[ 0 ].appendVertex( OdGePoint2d( 0, 0 ) );
  cProfile[ 0 ].appendVertex( OdGePoint2d( 1, 0 ) );
  cProfile[ 0 ].appendVertex( OdGePoint2d( 1, 1 ) );
  cProfile[ 0 ].appendVertex( OdGePoint2d( 0, 1 ) );
  cProfile[ 0 ].setClosed();

  // Forces the contour to be counter-clockwise.
  // So if the contour is already ccw this call is not needed.
  cProfile[ 0 ].makeCCW();

  cProfile[ 1 ].appendVertex( OdGePoint2d( 0.2, 0.2 ) );
  cProfile[ 1 ].appendVertex( OdGePoint2d( 0.2, 0.8 ) );
  cProfile[ 1 ].appendVertex( OdGePoint2d( 0.8, 0.8 ) );
  cProfile[ 1 ].appendVertex( OdGePoint2d( 0.8, 0.2 ) );
  cProfile[ 1 ].setClosed();

  cProfile[ 1 ].makeCCW( false );

  pProfileDefinition->GetProfile()->Init( cProfile );

  return( pProfileDefinition->objectId() );
}

// Inserts a door style into a database.
// Returns Object ID of newly created door style.
OdDbObjectId add_door_style( OdDbDatabasePtr pDatabase, const OdDbObjectId& idProfile )
{
  OdDbObjectId idDoorStyle =
    AECDbDoorStyle::CreateAECObject( pDatabase, OD_T("Door Style Created By Teigha(R) Architecture") );
  AECDbDoorStylePtr pDoorStyle = idDoorStyle.openObject( OdDb::kForWrite );

  pDoorStyle->SetDescription( OD_T("Door Style Description") );
  pDoorStyle->SetDictRecordDescription( OD_T("Dialog caption") );
  pDoorStyle->SetAutoAdjustToWidthOfWall( true );
  pDoorStyle->SetFrameWidth( 2 );
  pDoorStyle->SetFrameDepth( 5 );
  pDoorStyle->SetStopWidth( 2 );
  pDoorStyle->SetStopDepth( 3 );
  pDoorStyle->SetShapeAndType( AECDefs::esCustom, AECDefs::edtSingle );
  pDoorStyle->SetProfile( idProfile );
  pDoorStyle->SetGlassThickness( 1 );

  AECDisplayManager cDM( pDatabase );
  AECDbDispPropsDoorPtr pOverrideModel =
    AECDbDispPropsDoor::cast( pDoorStyle->OverrideDispProps(
    cDM.UpdateDisplayRepresentation( AECDbDispRepDoorModel::desc() ) ).openObject( OdDb::kForWrite ) );
  if ( !pOverrideModel.isNull() )
  {
    pOverrideModel->GetPanelComp()->SetColor( colorAt( 1 ) );
    pOverrideModel->GetFrameComp()->SetColor( colorAt( 2 ) );
    pOverrideModel->GetStopComp()->SetColor( colorAt( 3 ) );
    pOverrideModel->GetSwingComp()->SetColor( colorAt( 4 ) );
    pOverrideModel->GetGlassComp()->SetColor( colorAt( 5 ) );
  }

  AECDbDispPropsDoorPtr pOverridePlan =
    AECDbDispPropsDoor::cast( pDoorStyle->OverrideDispProps(
    cDM.UpdateDisplayRepresentation( AECDbDispRepDoorPlan::desc() ) ).openObject( OdDb::kForWrite ) );
  if ( !pOverridePlan.isNull() )
  {
    pOverridePlan->GetPanelComp()->SetColor( colorAt( 1 ) );
    pOverridePlan->GetFrameComp()->SetColor( colorAt( 2 ) );
    pOverridePlan->GetStopComp()->SetColor( colorAt( 3 ) );
    pOverridePlan->GetSwingComp()->SetColor( colorAt( 4 ) );
    pOverridePlan->GetDirectionComp()->SetColor( colorAt( 5 ) );
  }

  return( pDoorStyle->objectId() );
}


// Inserts a door into a database using the specified door style.
// If idWall parameter is not null it also attaches the door to the wall.
// Returns Object ID of newly created door.
OdDbObjectId add_door( OdDbDatabasePtr pDatabase, const OdDbObjectId& idStyle, const OdDbObjectId& idWall,
                      double dOffsetAlongX, double dOffsetAlongZ )
{
  AECDbDoorPtr pDoor = AECDbDoor::CreateAECObject( pDatabase->getModelSpaceId(), idStyle );

  pDoor->SetRise( 10 );
  pDoor->SetWidth( 40 );
  pDoor->SetHeight( 50 );

  pDoor->SetOpenPercent( 20 );
  pDoor->SetMeasureTo( AECDefs::eomtOutsideFrame );
  pDoor->SetLeaf( 10 );

  if ( !idWall.isNull() )
  {
    pDoor->AttachWallAnchor( idWall );

    AECDbAnchorEntToCurvePtr pAnchor = pDoor->GetAnchor().openObject( OdDb::kForWrite );
    pAnchor->GetXParams()->SetOffset( dOffsetAlongX );
    pAnchor->GetZParams()->SetOffset( dOffsetAlongZ );
  }

  return( pDoor->objectId() );
}


void add_roof( OdDbDatabasePtr pDatabase )
{
  AECGe::Profile2D cProfile;
  cProfile.resize( 1 );
  cProfile.front().appendVertex( OdGePoint2d( 0,   0   )    );
  cProfile.front().appendVertex( OdGePoint2d( 0, 110   )    );
  cProfile.front().appendVertex( OdGePoint2d( 110, 110   )    );
  cProfile.front().appendVertex( OdGePoint2d( 110, 0 ), -1   );
  cProfile.front().setClosed();
  cProfile.front().makeCCW();
  
  AECDbRoofPtr pRoof =
    AECDbRoof::CreateAECObject( pDatabase->getModelSpaceId() );

  // Initialize roof profile.
  // By default all edges of Roof Profile have single slope of 45 degrees.
  pRoof->GetProfile()->Init( cProfile );

  pRoof->SetThickness( 2 );

  //// Manually modify Roof Segments.
  AECGeRingSubPtr pRoofLoop = pRoof->GetProfile()->GetRingByIndex( 0 );
  if ( !pRoofLoop.isNull() )
  {
    OdUInt32 i, iSize = pRoofLoop->GetSegmentCount();
    for ( i = 0; i < iSize; i++ )
    {
      AECGeRoofSegmentSubPtr pSeg = pRoofLoop->GetSegments()->GetAt( i );
      pSeg->SetFaceCount(1);
      pSeg->SetFaceHeightByIndex(0, 110);
      pSeg->SetBaseHeight(0);
      pSeg->SetOverhang(10.0);
      pSeg->SetFaceSlopeByIndex(0, OdaPI4);
      pSeg->SetSegmentCount(10);
    }
  }

  pRoof->setColorIndex( 3 );  
}

void add_slab( OdDbDatabasePtr pDatabase )
{
  OdDbObjectId idStyle =
    AECDbSlabStyle::GetAECObject( pDatabase, OD_T("Slab Style") );
  if ( idStyle.isNull() )
  {
    idStyle = AECDbSlabStyle::CreateAECObject( pDatabase, OD_T("Slab Style") );
  }

  AECDbSlabStylePtr pStyle =
    idStyle.openObject( OdDb::kForWrite );
  if ( !pStyle.isNull() )
  {
    pStyle->GetComponents()->Clear();

    AECSlabStyleCompPtr pCmp = AECSlabStyleComp::createObject();
    pCmp->SetName( OD_T("Base") );
    pCmp->GetPosition()->GetThickness()->SetUseBaseValue( false );
    pCmp->GetPosition()->GetThickness()->SetBaseValue( 6 );
    pCmp->GetPosition()->GetThicknessOffset()->SetUseBaseValue( false );
    pCmp->GetPosition()->GetThicknessOffset()->SetBaseValue( - 6 );
    pStyle->GetComponents()->Insert( pCmp );
  }

  AECDbSlabPtr pSlab =
    AECDbSlab::CreateAECObject( pDatabase->getModelSpaceId(), idStyle );

  {
    AECGe::Profile2D cBase;
    cBase.resize( 1 );
    cBase.front().appendVertex( OdGePoint2d( -5,   -5   ), 1 );
    cBase.front().appendVertex( OdGePoint2d( 115, -5   ) );
    cBase.front().appendVertex( OdGePoint2d( 115, 115 ) );
    cBase.front().appendVertex( OdGePoint2d( -5,   115 ) );
    cBase.front().setClosed();
    cBase.front().makeCCW();

    pSlab->GetSlabFace()->Init( cBase );
  }

  pSlab->SetThickness( 5 );

  pSlab->SetVerticalOffset( 0 );
  pSlab->SetHorizontalOffset( 0 );

  pSlab->SetPivotPoint( OdGePoint3d::kOrigin );

  AECDisplayManager cDM( pDatabase );
  AECDbDispPropsSlabPtr pOverrideModel =
    AECDbDispPropsSlab::cast( pSlab->OverrideDispProps(
    cDM.UpdateDisplayRepresentation( AECDbDispRepSlabModel::desc() ) ).openObject( OdDb::kForWrite ) );
  if ( !pOverrideModel.isNull() )
  {
    pOverrideModel->GetBoundaryCompByIndex( 0 )->SetColor( colorAt( 1 ) );
    pOverrideModel->GetBaselineComp()->SetColor( colorAt( 4 ) );
    pOverrideModel->GetPivotPointComp()->SetColor( colorAt( 5 ) );
    pOverrideModel->GetFasciaComp()->SetColor( colorAt( 6 ) );
    pOverrideModel->GetSoffitComp()->SetColor( colorAt( 7 ) );
    pOverrideModel->GetShrinkWrapBodyComp()->SetColor( colorAt( 8 ) );
  }

  AECDbDispPropsSlabPlanPtr pOverridePlan =
    AECDbDispPropsSlabPlan::cast( pSlab->OverrideDispProps(
    cDM.UpdateDisplayRepresentation( AECDbDispRepSlabPlan::desc() ) ).openObject( OdDb::kForWrite ) );
  if ( !pOverridePlan.isNull() )
  {
    pOverridePlan->SetIsOverrideCutPlane( false );
    pOverridePlan->GetHatchComp()->SetColor( colorAt( 1 ) );
    pOverridePlan->GetBelowCutPlaneBodyComp()->SetColor( colorAt( 2 ) );
    pOverridePlan->GetAboveCutPlaneBodyComp()->SetColor( colorAt( 3 ) );
    pOverridePlan->GetBelowCutPlaneOutlineComp()->SetColor( colorAt( 4 ) );
    pOverridePlan->GetAboveCutPlaneOutlineComp()->SetColor( colorAt( 5 ) );
  }
}

class MyServices : public ExSystemServices, public ExHostAppServices
{
protected:
  ODRX_USING_HEAP_OPERATORS(ExSystemServices);
};


int wmain(int argc, wchar_t* argv[])
{ 
  // Initialize TD with system services.
  // And create single instance of hostapp services
  // for TD database creation.
  OdStaticRxObject<MyServices> svcs;
  odInitialize( &svcs );
  odgsInitialize();


  // Loading of all public Teigha Architecture DRX modules.
  // Note that not all calls are necessary for some of them depend on others
  // but here we list all of them.
  //
  // If a program uses TD doesn't modify or create binary files
  // it may not load any of DRX modules on start because they will be loaded automatically. 
  // But if a program modifies or creates binary files then it is highly recommended
  // to load all DRX modules program uses.
  ::odrxDynamicLinker()->loadApp( OD_T("AecBase") );
  ::odrxDynamicLinker()->loadApp( OD_T("AecArchBase") );
  ::odrxDynamicLinker()->loadApp( OD_T("AecArchDACHBase") );
  ::odrxDynamicLinker()->loadApp( OD_T("AecScheduleData") );
  ::odrxDynamicLinker()->loadApp( OD_T("AecSchedule") );
  ::odrxDynamicLinker()->loadApp( OD_T("AecStructureBase") );

  // Create empty TD database.
  OdDbDatabasePtr pDatabase = svcs.createDatabase();;
  
  // Initialize database with default Teigha Architecture content.
  AECArchDACHBaseDatabase( pDatabase ).Init();
  AECScheduleDatabase( pDatabase ).Init();
  AECStructureBaseDatabase( pDatabase ).Init();


  init_display_system( pDatabase );

    
  OdDbObjectId idWallStyle = add_wall_style( pDatabase );

  OdDbObjectId idWall1 = add_wall( pDatabase, idWallStyle,
    OdGePoint2d( 0,     0 ), OdGePoint2d(   0, 110 ) );
  OdDbObjectId idWall2 = add_wall( pDatabase, idWallStyle,
    OdGePoint2d( 0,   110 ), OdGePoint2d( 110, 110 ) );
  OdDbObjectId idWall3 = add_wall( pDatabase, idWallStyle,
    OdGePoint2d( 110, 110 ), OdGePoint2d( 110,   0 ) );
  OdDbObjectId idWall4 = add_wall( pDatabase, idWallStyle,
    OdGePoint2d( 110,   0 ), OdGePoint2d(   0,   0 ), -1 );

  AECDbWallPtr ptWall = AECDbWall::cast( idWall1.openObject( OdDb::kForRead ) ); 

  OdDbObjectId idWindowStyle = add_window_style( pDatabase );

  OdDbObjectId idWindow01 = add_window( pDatabase, idWindowStyle, idWall1, 10, 10 );
  OdDbObjectId idWindow02 = add_window( pDatabase, idWindowStyle, idWall1, 60, 10 );
  OdDbObjectId idWindow03 = add_window( pDatabase, idWindowStyle, idWall1, 10, 60 );
  OdDbObjectId idWindow04 = add_window( pDatabase, idWindowStyle, idWall1, 60, 60 );

  OdDbObjectId idWindow05 = add_window( pDatabase, idWindowStyle, idWall2, 10, 10 );
  OdDbObjectId idWindow06 = add_window( pDatabase, idWindowStyle, idWall2, 60, 10 );
  OdDbObjectId idWindow07 = add_window( pDatabase, idWindowStyle, idWall2, 10, 60 );
  OdDbObjectId idWindow08 = add_window( pDatabase, idWindowStyle, idWall2, 60, 60 );

  OdDbObjectId idWindow09 = add_window( pDatabase, idWindowStyle, idWall3, 10, 10 );
  OdDbObjectId idWindow10 = add_window( pDatabase, idWindowStyle, idWall3, 60, 10 );
  OdDbObjectId idWindow11 = add_window( pDatabase, idWindowStyle, idWall3, 10, 60 );
  OdDbObjectId idWindow12 = add_window( pDatabase, idWindowStyle, idWall3, 60, 60 );

  OdDbObjectId idProfile =
    add_profile_def( pDatabase );

  OdDbObjectId idDoorStyle =
    add_door_style( pDatabase, idProfile );

  AECDbWallPtr pWall = idWall4.openObject( OdDb::kForRead );
  double dLength = pWall->GetLength();
  double dOWidth = 40;
  double dL1 = 10;
  double dL3 = dLength - dOWidth - 10;
  double dL2 = dL1 + dOWidth + (dL3 - (dL1 + 2 * dOWidth)) / 2;

  OdDbObjectId idDoor     = add_door   ( pDatabase, idDoorStyle,   idWall4, dL2, 0  );
  OdDbObjectId idWindow13 = add_window ( pDatabase, idWindowStyle, idWall4, dL1, 10 );
  OdDbObjectId idWindow14 = add_window ( pDatabase, idWindowStyle, idWall4, dL3, 10 );
  OdDbObjectId idWindow15 = add_window ( pDatabase, idWindowStyle, idWall4, dL1, 60 );
  OdDbObjectId idWindow16 = add_window ( pDatabase, idWindowStyle, idWall4, dL2, 60 );
  OdDbObjectId idOpening  = add_window ( pDatabase, idWindowStyle, idWall4, dL3, 60 );

  add_roof(pDatabase);
  add_slab(pDatabase);


  // Perform "zoom extents" on model space.
  {
    OdDbViewportTablePtr pVT =
      pDatabase->getViewportTableId().openObject( OdDb::kForRead );
    OdDbViewportTableRecordPtr pV =
      pVT->getActiveViewportId().openObject( OdDb::kForWrite );
    pV->zoomExtents();
  }

  OdWrFileBuf cBuffer( "H:\\TA_test.dwg" );
  pDatabase->writeFile( &cBuffer, OdDb::kDwg, OdDb::kDHL_CURRENT );
  
  odgsUninitialize();
  odUninitialize();

  return 0;
}