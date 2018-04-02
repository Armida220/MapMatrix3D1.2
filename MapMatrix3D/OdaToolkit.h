/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Thu Dec 16 22:11:10 2004
 */
/* Compiler settings for D:\OpenDWG\dd1.12\cpp\NewToolkit\OdaToolkit\OdaToolkit.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __OdaToolkit_h__
#define __OdaToolkit_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IAcadApplication_FWD_DEFINED__
#define __IAcadApplication_FWD_DEFINED__
typedef interface IAcadApplication IAcadApplication;
#endif 	/* __IAcadApplication_FWD_DEFINED__ */


#ifndef __IAcadUtility_FWD_DEFINED__
#define __IAcadUtility_FWD_DEFINED__
typedef interface IAcadUtility IAcadUtility;
#endif 	/* __IAcadUtility_FWD_DEFINED__ */


#ifndef __IAcadDocument_FWD_DEFINED__
#define __IAcadDocument_FWD_DEFINED__
typedef interface IAcadDocument IAcadDocument;
#endif 	/* __IAcadDocument_FWD_DEFINED__ */


#ifndef __IAcadDocuments_FWD_DEFINED__
#define __IAcadDocuments_FWD_DEFINED__
typedef interface IAcadDocuments IAcadDocuments;
#endif 	/* __IAcadDocuments_FWD_DEFINED__ */


#ifndef __IAcadSelectionSets_FWD_DEFINED__
#define __IAcadSelectionSets_FWD_DEFINED__
typedef interface IAcadSelectionSets IAcadSelectionSets;
#endif 	/* __IAcadSelectionSets_FWD_DEFINED__ */


#ifndef __IAcadSelectionSet_FWD_DEFINED__
#define __IAcadSelectionSet_FWD_DEFINED__
typedef interface IAcadSelectionSet IAcadSelectionSet;
#endif 	/* __IAcadSelectionSet_FWD_DEFINED__ */


#ifndef __IAcadPreferences_FWD_DEFINED__
#define __IAcadPreferences_FWD_DEFINED__
typedef interface IAcadPreferences IAcadPreferences;
#endif 	/* __IAcadPreferences_FWD_DEFINED__ */


#ifndef __IAcadPreferencesFiles_FWD_DEFINED__
#define __IAcadPreferencesFiles_FWD_DEFINED__
typedef interface IAcadPreferencesFiles IAcadPreferencesFiles;
#endif 	/* __IAcadPreferencesFiles_FWD_DEFINED__ */


#ifndef __IAcadLayerStateManager_FWD_DEFINED__
#define __IAcadLayerStateManager_FWD_DEFINED__
typedef interface IAcadLayerStateManager IAcadLayerStateManager;
#endif 	/* __IAcadLayerStateManager_FWD_DEFINED__ */


#ifndef __IAcadSecurityParams_FWD_DEFINED__
#define __IAcadSecurityParams_FWD_DEFINED__
typedef interface IAcadSecurityParams IAcadSecurityParams;
#endif 	/* __IAcadSecurityParams_FWD_DEFINED__ */


#ifndef __IOdaSvgExporter_FWD_DEFINED__
#define __IOdaSvgExporter_FWD_DEFINED__
typedef interface IOdaSvgExporter IOdaSvgExporter;
#endif 	/* __IOdaSvgExporter_FWD_DEFINED__ */


#ifndef __IOdaAuditInfo_FWD_DEFINED__
#define __IOdaAuditInfo_FWD_DEFINED__
typedef interface IOdaAuditInfo IOdaAuditInfo;
#endif 	/* __IOdaAuditInfo_FWD_DEFINED__ */


#ifndef ___DAcadApplicationEvents_FWD_DEFINED__
#define ___DAcadApplicationEvents_FWD_DEFINED__
typedef interface _DAcadApplicationEvents _DAcadApplicationEvents;
#endif 	/* ___DAcadApplicationEvents_FWD_DEFINED__ */


#ifndef ___DAcadDocumentEvents_FWD_DEFINED__
#define ___DAcadDocumentEvents_FWD_DEFINED__
typedef interface _DAcadDocumentEvents _DAcadDocumentEvents;
#endif 	/* ___DAcadDocumentEvents_FWD_DEFINED__ */


#ifndef __IOdaHostApp_FWD_DEFINED__
#define __IOdaHostApp_FWD_DEFINED__
typedef interface IOdaHostApp IOdaHostApp;
#endif 	/* __IOdaHostApp_FWD_DEFINED__ */


#ifndef __IOdaDwfPageData_FWD_DEFINED__
#define __IOdaDwfPageData_FWD_DEFINED__
typedef interface IOdaDwfPageData IOdaDwfPageData;
#endif 	/* __IOdaDwfPageData_FWD_DEFINED__ */


#ifndef __IOdaDwfTemplate_FWD_DEFINED__
#define __IOdaDwfTemplate_FWD_DEFINED__
typedef interface IOdaDwfTemplate IOdaDwfTemplate;
#endif 	/* __IOdaDwfTemplate_FWD_DEFINED__ */


#ifndef __IOdaDwfImporter_FWD_DEFINED__
#define __IOdaDwfImporter_FWD_DEFINED__
typedef interface IOdaDwfImporter IOdaDwfImporter;
#endif 	/* __IOdaDwfImporter_FWD_DEFINED__ */


#ifndef __AcadApplication_FWD_DEFINED__
#define __AcadApplication_FWD_DEFINED__

#ifdef __cplusplus
typedef class AcadApplication AcadApplication;
#else
typedef struct AcadApplication AcadApplication;
#endif /* __cplusplus */

#endif 	/* __AcadApplication_FWD_DEFINED__ */


#ifndef __AcadDocument_FWD_DEFINED__
#define __AcadDocument_FWD_DEFINED__

#ifdef __cplusplus
typedef class AcadDocument AcadDocument;
#else
typedef struct AcadDocument AcadDocument;
#endif /* __cplusplus */

#endif 	/* __AcadDocument_FWD_DEFINED__ */


#ifndef ___IOdaHostAppEvents_FWD_DEFINED__
#define ___IOdaHostAppEvents_FWD_DEFINED__
typedef interface _IOdaHostAppEvents _IOdaHostAppEvents;
#endif 	/* ___IOdaHostAppEvents_FWD_DEFINED__ */


#ifndef __OdaHostApp_FWD_DEFINED__
#define __OdaHostApp_FWD_DEFINED__

#ifdef __cplusplus
typedef class OdaHostApp OdaHostApp;
#else
typedef struct OdaHostApp OdaHostApp;
#endif /* __cplusplus */

#endif 	/* __OdaHostApp_FWD_DEFINED__ */


#ifndef __AcadUtility_FWD_DEFINED__
#define __AcadUtility_FWD_DEFINED__

#ifdef __cplusplus
typedef class AcadUtility AcadUtility;
#else
typedef struct AcadUtility AcadUtility;
#endif /* __cplusplus */

#endif 	/* __AcadUtility_FWD_DEFINED__ */


#ifndef __AcadPreferences_FWD_DEFINED__
#define __AcadPreferences_FWD_DEFINED__

#ifdef __cplusplus
typedef class AcadPreferences AcadPreferences;
#else
typedef struct AcadPreferences AcadPreferences;
#endif /* __cplusplus */

#endif 	/* __AcadPreferences_FWD_DEFINED__ */


#ifndef __AcadSelectionSet_FWD_DEFINED__
#define __AcadSelectionSet_FWD_DEFINED__

#ifdef __cplusplus
typedef class AcadSelectionSet AcadSelectionSet;
#else
typedef struct AcadSelectionSet AcadSelectionSet;
#endif /* __cplusplus */

#endif 	/* __AcadSelectionSet_FWD_DEFINED__ */


#ifndef __OdaDwfTemplate_FWD_DEFINED__
#define __OdaDwfTemplate_FWD_DEFINED__

#ifdef __cplusplus
typedef class OdaDwfTemplate OdaDwfTemplate;
#else
typedef struct OdaDwfTemplate OdaDwfTemplate;
#endif /* __cplusplus */

#endif 	/* __OdaDwfTemplate_FWD_DEFINED__ */


#ifndef __OdaDwfImporter_FWD_DEFINED__
#define __OdaDwfImporter_FWD_DEFINED__

#ifdef __cplusplus
typedef class OdaDwfImporter OdaDwfImporter;
#else
typedef struct OdaDwfImporter OdaDwfImporter;
#endif /* __cplusplus */

#endif 	/* __OdaDwfImporter_FWD_DEFINED__ */


#ifndef __AcadLayerStateManager_FWD_DEFINED__
#define __AcadLayerStateManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class AcadLayerStateManager AcadLayerStateManager;
#else
typedef struct AcadLayerStateManager AcadLayerStateManager;
#endif /* __cplusplus */

#endif 	/* __AcadLayerStateManager_FWD_DEFINED__ */


#ifndef __AcadSecurityParams_FWD_DEFINED__
#define __AcadSecurityParams_FWD_DEFINED__

#ifdef __cplusplus
typedef class AcadSecurityParams AcadSecurityParams;
#else
typedef struct AcadSecurityParams AcadSecurityParams;
#endif /* __cplusplus */

#endif 	/* __AcadSecurityParams_FWD_DEFINED__ */


#ifndef __OdaSvgExporter_FWD_DEFINED__
#define __OdaSvgExporter_FWD_DEFINED__

#ifdef __cplusplus
typedef class OdaSvgExporter OdaSvgExporter;
#else
typedef struct OdaSvgExporter OdaSvgExporter;
#endif /* __cplusplus */

#endif 	/* __OdaSvgExporter_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "odax.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __DWGdirectX_LIBRARY_DEFINED__
#define __DWGdirectX_LIBRARY_DEFINED__

/* library DWGdirectX */
/* [helpstring][version][uuid] */ 



























typedef /* [public][uuid] */ 
enum __MIDL___MIDL_itf_OdaToolkit_0000_0001
    {	odR09_dxf	= -5,
	odR10_dxf	= -4,
	odR12_dwg	= -3,
	odUnknown	= -1,
	odR12_dxf	= 1,
	odR13_dwg	= 4,
	odR13_dxf	= 5,
	odR14_dwg	= 8,
	odR14_dxf	= 9,
	odR15_dwg	= 12,
	odR15_dxf	= 13,
	odR15_Template	= 14,
	odNative	= 24,
	odR18_dwg	= 24,
	odR18_dxf	= 25,
	odR18_Template	= 26
    }	OdSaveAsType;

typedef /* [public][uuid] */ 
enum __MIDL___MIDL_itf_OdaToolkit_0312_0001
    {	ACADSECURITYPARAMS_ENCRYPT_DATA	= 1,
	ACADSECURITYPARAMS_ENCRYPT_PROPS	= 2,
	ACADSECURITYPARAMS_SIGN_DATA	= 16,
	ACADSECURITYPARAMS_ADD_TIMESTAMP	= 32
    }	AcadSecurityParamsType;

typedef /* [public][uuid] */ 
enum __MIDL___MIDL_itf_OdaToolkit_0312_0002
    {	ACADSECURITYPARAMS_ALGID_RC4	= 26625
    }	AcadSecurityParamsConstants;

typedef /* [public][public][public][uuid] */ 
enum __MIDL___MIDL_itf_OdaToolkit_0324_0001
    {	odDwfCompressedBinary	= 0,
	odDwfUncompressedBinary	= 1,
	odDwfAscii	= 2
    }	OdDwfFormat;

typedef /* [public][public][public][uuid] */ 
enum __MIDL___MIDL_itf_OdaToolkit_0324_0002
    {	odDwf_v55	= 55,
	odDwf_v42	= 42,
	odDwf_v60	= 600
    }	OdDwfVersion;

typedef /* [public][public][public][public][uuid] */ 
enum __MIDL___MIDL_itf_OdaToolkit_0328_0001
    {	acLsNone	= 0,
	acLsOn	= 1,
	acLsFrozen	= 2,
	acLsLocked	= 4,
	acLsPlot	= 8,
	acLsNewViewport	= 16,
	acLsColor	= 32,
	acLsLineType	= 64,
	acLsLineWeight	= 128,
	acLsPlotStyle	= 256,
	acLsAll	= 65535
    }	AcLayerStateMask;


EXTERN_C const IID LIBID_DWGdirectX;

#ifndef __IAcadApplication_INTERFACE_DEFINED__
#define __IAcadApplication_INTERFACE_DEFINED__

/* interface IAcadApplication */
/* [oleautomation][dual][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IAcadApplication;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("93BC4E71-AFE7-4AA7-BC07-F80ACDB672D5")
    IAcadApplication : public IDispatch
    {
    public:
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Visible( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Visible) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Visible( 
            /* [in] */ VARIANT_BOOL Visible) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pAppName) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Caption( 
            /* [retval][out] */ BSTR __RPC_FAR *bstrCaption) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IAcadApplication __RPC_FAR *__RPC_FAR *pAppObj) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActiveDocument( 
            /* [retval][out] */ IAcadDocument __RPC_FAR *__RPC_FAR *pActiveDoc) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ActiveDocument( 
            /* [in] */ IAcadDocument __RPC_FAR *pActiveDoc) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_FullName( 
            /* [retval][out] */ BSTR __RPC_FAR *FullName) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Height( 
            /* [retval][out] */ int __RPC_FAR *Height) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Height( 
            /* [in] */ int Height) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_WindowLeft( 
            /* [retval][out] */ int __RPC_FAR *left) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_WindowLeft( 
            /* [in] */ int left) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Path( 
            /* [retval][out] */ BSTR __RPC_FAR *bstrPath) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_LocaleId( 
            /* [retval][out] */ long __RPC_FAR *lcid) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_WindowTop( 
            /* [retval][out] */ int __RPC_FAR *top) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_WindowTop( 
            /* [in] */ int top) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Version( 
            /* [retval][out] */ BSTR __RPC_FAR *bstrVer) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Width( 
            /* [retval][out] */ int __RPC_FAR *Width) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Width( 
            /* [in] */ int Width) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Preferences( 
            /* [retval][out] */ IAcadPreferences __RPC_FAR *__RPC_FAR *pPreferences) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_StatusId( 
            /* [in] */ IDispatch __RPC_FAR *VportObj,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *bStatus) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ListArx( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVarListArray) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadArx( 
            /* [in] */ BSTR Name) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetInterfaceObject( 
            /* [in] */ BSTR ProgID,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pObj) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnloadArx( 
            /* [in] */ BSTR Name) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Update( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Quit( void) = 0;
        
        virtual /* [helpstring][hidden][id] */ HRESULT STDMETHODCALLTYPE Zoom( 
            /* [in] */ int Type,
            /* [in] */ VARIANT __RPC_FAR *vParams) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_VBE( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pDispVBE) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_MenuGroups( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pMenuGroups) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_MenuBar( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pMenuBar) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadDVB( 
            /* [in] */ BSTR Name) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnloadDVB( 
            /* [in] */ BSTR Name) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Documents( 
            /* [retval][out] */ IAcadDocuments __RPC_FAR *__RPC_FAR *pDocuments) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Eval( 
            /* [in] */ BSTR Expression) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_WindowState( 
            /* [retval][out] */ AcWindowState __RPC_FAR *eWinState) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_WindowState( 
            /* [in] */ AcWindowState eWinState) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RunMacro( 
            /* [in] */ BSTR MacroPath) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ZoomExtents( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ZoomAll( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ZoomCenter( 
            /* [in] */ VARIANT Center,
            /* [in] */ double Magnify) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ZoomScaled( 
            /* [in] */ double scale,
            /* [in] */ AcZoomScaleType ScaleType) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ZoomWindow( 
            /* [in] */ VARIANT LowerLeft,
            /* [in] */ VARIANT UpperRight) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ZoomPickWindow( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAcadState( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ZoomPrevious( void) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_HWND( 
            /* [retval][out] */ long __RPC_FAR *HWND) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAcadApplicationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAcadApplication __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAcadApplication __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAcadApplication __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAcadApplication __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAcadApplication __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAcadApplication __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAcadApplication __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Visible )( 
            IAcadApplication __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Visible);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Visible )( 
            IAcadApplication __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Visible);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IAcadApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pAppName);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Caption )( 
            IAcadApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *bstrCaption);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Application )( 
            IAcadApplication __RPC_FAR * This,
            /* [retval][out] */ IAcadApplication __RPC_FAR *__RPC_FAR *pAppObj);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveDocument )( 
            IAcadApplication __RPC_FAR * This,
            /* [retval][out] */ IAcadDocument __RPC_FAR *__RPC_FAR *pActiveDoc);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ActiveDocument )( 
            IAcadApplication __RPC_FAR * This,
            /* [in] */ IAcadDocument __RPC_FAR *pActiveDoc);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FullName )( 
            IAcadApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *FullName);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Height )( 
            IAcadApplication __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *Height);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Height )( 
            IAcadApplication __RPC_FAR * This,
            /* [in] */ int Height);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WindowLeft )( 
            IAcadApplication __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *left);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WindowLeft )( 
            IAcadApplication __RPC_FAR * This,
            /* [in] */ int left);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Path )( 
            IAcadApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *bstrPath);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LocaleId )( 
            IAcadApplication __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *lcid);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WindowTop )( 
            IAcadApplication __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *top);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WindowTop )( 
            IAcadApplication __RPC_FAR * This,
            /* [in] */ int top);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Version )( 
            IAcadApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *bstrVer);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Width )( 
            IAcadApplication __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *Width);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Width )( 
            IAcadApplication __RPC_FAR * This,
            /* [in] */ int Width);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Preferences )( 
            IAcadApplication __RPC_FAR * This,
            /* [retval][out] */ IAcadPreferences __RPC_FAR *__RPC_FAR *pPreferences);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_StatusId )( 
            IAcadApplication __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *VportObj,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *bStatus);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ListArx )( 
            IAcadApplication __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVarListArray);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LoadArx )( 
            IAcadApplication __RPC_FAR * This,
            /* [in] */ BSTR Name);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetInterfaceObject )( 
            IAcadApplication __RPC_FAR * This,
            /* [in] */ BSTR ProgID,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pObj);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UnloadArx )( 
            IAcadApplication __RPC_FAR * This,
            /* [in] */ BSTR Name);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Update )( 
            IAcadApplication __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Quit )( 
            IAcadApplication __RPC_FAR * This);
        
        /* [helpstring][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Zoom )( 
            IAcadApplication __RPC_FAR * This,
            /* [in] */ int Type,
            /* [in] */ VARIANT __RPC_FAR *vParams);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VBE )( 
            IAcadApplication __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pDispVBE);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MenuGroups )( 
            IAcadApplication __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pMenuGroups);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MenuBar )( 
            IAcadApplication __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pMenuBar);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LoadDVB )( 
            IAcadApplication __RPC_FAR * This,
            /* [in] */ BSTR Name);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UnloadDVB )( 
            IAcadApplication __RPC_FAR * This,
            /* [in] */ BSTR Name);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Documents )( 
            IAcadApplication __RPC_FAR * This,
            /* [retval][out] */ IAcadDocuments __RPC_FAR *__RPC_FAR *pDocuments);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Eval )( 
            IAcadApplication __RPC_FAR * This,
            /* [in] */ BSTR Expression);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WindowState )( 
            IAcadApplication __RPC_FAR * This,
            /* [retval][out] */ AcWindowState __RPC_FAR *eWinState);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WindowState )( 
            IAcadApplication __RPC_FAR * This,
            /* [in] */ AcWindowState eWinState);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RunMacro )( 
            IAcadApplication __RPC_FAR * This,
            /* [in] */ BSTR MacroPath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ZoomExtents )( 
            IAcadApplication __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ZoomAll )( 
            IAcadApplication __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ZoomCenter )( 
            IAcadApplication __RPC_FAR * This,
            /* [in] */ VARIANT Center,
            /* [in] */ double Magnify);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ZoomScaled )( 
            IAcadApplication __RPC_FAR * This,
            /* [in] */ double scale,
            /* [in] */ AcZoomScaleType ScaleType);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ZoomWindow )( 
            IAcadApplication __RPC_FAR * This,
            /* [in] */ VARIANT LowerLeft,
            /* [in] */ VARIANT UpperRight);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ZoomPickWindow )( 
            IAcadApplication __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAcadState )( 
            IAcadApplication __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ZoomPrevious )( 
            IAcadApplication __RPC_FAR * This);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HWND )( 
            IAcadApplication __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *HWND);
        
        END_INTERFACE
    } IAcadApplicationVtbl;

    interface IAcadApplication
    {
        CONST_VTBL struct IAcadApplicationVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAcadApplication_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAcadApplication_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAcadApplication_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAcadApplication_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAcadApplication_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAcadApplication_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAcadApplication_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAcadApplication_get_Visible(This,Visible)	\
    (This)->lpVtbl -> get_Visible(This,Visible)

#define IAcadApplication_put_Visible(This,Visible)	\
    (This)->lpVtbl -> put_Visible(This,Visible)

#define IAcadApplication_get_Name(This,pAppName)	\
    (This)->lpVtbl -> get_Name(This,pAppName)

#define IAcadApplication_get_Caption(This,bstrCaption)	\
    (This)->lpVtbl -> get_Caption(This,bstrCaption)

#define IAcadApplication_get_Application(This,pAppObj)	\
    (This)->lpVtbl -> get_Application(This,pAppObj)

#define IAcadApplication_get_ActiveDocument(This,pActiveDoc)	\
    (This)->lpVtbl -> get_ActiveDocument(This,pActiveDoc)

#define IAcadApplication_put_ActiveDocument(This,pActiveDoc)	\
    (This)->lpVtbl -> put_ActiveDocument(This,pActiveDoc)

#define IAcadApplication_get_FullName(This,FullName)	\
    (This)->lpVtbl -> get_FullName(This,FullName)

#define IAcadApplication_get_Height(This,Height)	\
    (This)->lpVtbl -> get_Height(This,Height)

#define IAcadApplication_put_Height(This,Height)	\
    (This)->lpVtbl -> put_Height(This,Height)

#define IAcadApplication_get_WindowLeft(This,left)	\
    (This)->lpVtbl -> get_WindowLeft(This,left)

#define IAcadApplication_put_WindowLeft(This,left)	\
    (This)->lpVtbl -> put_WindowLeft(This,left)

#define IAcadApplication_get_Path(This,bstrPath)	\
    (This)->lpVtbl -> get_Path(This,bstrPath)

#define IAcadApplication_get_LocaleId(This,lcid)	\
    (This)->lpVtbl -> get_LocaleId(This,lcid)

#define IAcadApplication_get_WindowTop(This,top)	\
    (This)->lpVtbl -> get_WindowTop(This,top)

#define IAcadApplication_put_WindowTop(This,top)	\
    (This)->lpVtbl -> put_WindowTop(This,top)

#define IAcadApplication_get_Version(This,bstrVer)	\
    (This)->lpVtbl -> get_Version(This,bstrVer)

#define IAcadApplication_get_Width(This,Width)	\
    (This)->lpVtbl -> get_Width(This,Width)

#define IAcadApplication_put_Width(This,Width)	\
    (This)->lpVtbl -> put_Width(This,Width)

#define IAcadApplication_get_Preferences(This,pPreferences)	\
    (This)->lpVtbl -> get_Preferences(This,pPreferences)

#define IAcadApplication_get_StatusId(This,VportObj,bStatus)	\
    (This)->lpVtbl -> get_StatusId(This,VportObj,bStatus)

#define IAcadApplication_ListArx(This,pVarListArray)	\
    (This)->lpVtbl -> ListArx(This,pVarListArray)

#define IAcadApplication_LoadArx(This,Name)	\
    (This)->lpVtbl -> LoadArx(This,Name)

#define IAcadApplication_GetInterfaceObject(This,ProgID,pObj)	\
    (This)->lpVtbl -> GetInterfaceObject(This,ProgID,pObj)

#define IAcadApplication_UnloadArx(This,Name)	\
    (This)->lpVtbl -> UnloadArx(This,Name)

#define IAcadApplication_Update(This)	\
    (This)->lpVtbl -> Update(This)

#define IAcadApplication_Quit(This)	\
    (This)->lpVtbl -> Quit(This)

#define IAcadApplication_Zoom(This,Type,vParams)	\
    (This)->lpVtbl -> Zoom(This,Type,vParams)

#define IAcadApplication_get_VBE(This,pDispVBE)	\
    (This)->lpVtbl -> get_VBE(This,pDispVBE)

#define IAcadApplication_get_MenuGroups(This,pMenuGroups)	\
    (This)->lpVtbl -> get_MenuGroups(This,pMenuGroups)

#define IAcadApplication_get_MenuBar(This,pMenuBar)	\
    (This)->lpVtbl -> get_MenuBar(This,pMenuBar)

#define IAcadApplication_LoadDVB(This,Name)	\
    (This)->lpVtbl -> LoadDVB(This,Name)

#define IAcadApplication_UnloadDVB(This,Name)	\
    (This)->lpVtbl -> UnloadDVB(This,Name)

#define IAcadApplication_get_Documents(This,pDocuments)	\
    (This)->lpVtbl -> get_Documents(This,pDocuments)

#define IAcadApplication_Eval(This,Expression)	\
    (This)->lpVtbl -> Eval(This,Expression)

#define IAcadApplication_get_WindowState(This,eWinState)	\
    (This)->lpVtbl -> get_WindowState(This,eWinState)

#define IAcadApplication_put_WindowState(This,eWinState)	\
    (This)->lpVtbl -> put_WindowState(This,eWinState)

#define IAcadApplication_RunMacro(This,MacroPath)	\
    (This)->lpVtbl -> RunMacro(This,MacroPath)

#define IAcadApplication_ZoomExtents(This)	\
    (This)->lpVtbl -> ZoomExtents(This)

#define IAcadApplication_ZoomAll(This)	\
    (This)->lpVtbl -> ZoomAll(This)

#define IAcadApplication_ZoomCenter(This,Center,Magnify)	\
    (This)->lpVtbl -> ZoomCenter(This,Center,Magnify)

#define IAcadApplication_ZoomScaled(This,scale,ScaleType)	\
    (This)->lpVtbl -> ZoomScaled(This,scale,ScaleType)

#define IAcadApplication_ZoomWindow(This,LowerLeft,UpperRight)	\
    (This)->lpVtbl -> ZoomWindow(This,LowerLeft,UpperRight)

#define IAcadApplication_ZoomPickWindow(This)	\
    (This)->lpVtbl -> ZoomPickWindow(This)

#define IAcadApplication_GetAcadState(This,pVal)	\
    (This)->lpVtbl -> GetAcadState(This,pVal)

#define IAcadApplication_ZoomPrevious(This)	\
    (This)->lpVtbl -> ZoomPrevious(This)

#define IAcadApplication_get_HWND(This,HWND)	\
    (This)->lpVtbl -> get_HWND(This,HWND)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_Visible_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Visible);


void __RPC_STUB IAcadApplication_get_Visible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_put_Visible_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL Visible);


void __RPC_STUB IAcadApplication_put_Visible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_Name_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pAppName);


void __RPC_STUB IAcadApplication_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_Caption_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *bstrCaption);


void __RPC_STUB IAcadApplication_get_Caption_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_Application_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [retval][out] */ IAcadApplication __RPC_FAR *__RPC_FAR *pAppObj);


void __RPC_STUB IAcadApplication_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_ActiveDocument_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [retval][out] */ IAcadDocument __RPC_FAR *__RPC_FAR *pActiveDoc);


void __RPC_STUB IAcadApplication_get_ActiveDocument_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_put_ActiveDocument_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [in] */ IAcadDocument __RPC_FAR *pActiveDoc);


void __RPC_STUB IAcadApplication_put_ActiveDocument_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_FullName_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *FullName);


void __RPC_STUB IAcadApplication_get_FullName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_Height_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *Height);


void __RPC_STUB IAcadApplication_get_Height_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_put_Height_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [in] */ int Height);


void __RPC_STUB IAcadApplication_put_Height_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_WindowLeft_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *left);


void __RPC_STUB IAcadApplication_get_WindowLeft_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_put_WindowLeft_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [in] */ int left);


void __RPC_STUB IAcadApplication_put_WindowLeft_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_Path_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *bstrPath);


void __RPC_STUB IAcadApplication_get_Path_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_LocaleId_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *lcid);


void __RPC_STUB IAcadApplication_get_LocaleId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_WindowTop_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *top);


void __RPC_STUB IAcadApplication_get_WindowTop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_put_WindowTop_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [in] */ int top);


void __RPC_STUB IAcadApplication_put_WindowTop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_Version_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *bstrVer);


void __RPC_STUB IAcadApplication_get_Version_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_Width_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *Width);


void __RPC_STUB IAcadApplication_get_Width_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_put_Width_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [in] */ int Width);


void __RPC_STUB IAcadApplication_put_Width_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_Preferences_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [retval][out] */ IAcadPreferences __RPC_FAR *__RPC_FAR *pPreferences);


void __RPC_STUB IAcadApplication_get_Preferences_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_StatusId_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [in] */ IDispatch __RPC_FAR *VportObj,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *bStatus);


void __RPC_STUB IAcadApplication_get_StatusId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_ListArx_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVarListArray);


void __RPC_STUB IAcadApplication_ListArx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_LoadArx_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [in] */ BSTR Name);


void __RPC_STUB IAcadApplication_LoadArx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_GetInterfaceObject_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [in] */ BSTR ProgID,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pObj);


void __RPC_STUB IAcadApplication_GetInterfaceObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_UnloadArx_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [in] */ BSTR Name);


void __RPC_STUB IAcadApplication_UnloadArx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_Update_Proxy( 
    IAcadApplication __RPC_FAR * This);


void __RPC_STUB IAcadApplication_Update_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_Quit_Proxy( 
    IAcadApplication __RPC_FAR * This);


void __RPC_STUB IAcadApplication_Quit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][hidden][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_Zoom_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [in] */ int Type,
    /* [in] */ VARIANT __RPC_FAR *vParams);


void __RPC_STUB IAcadApplication_Zoom_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_VBE_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pDispVBE);


void __RPC_STUB IAcadApplication_get_VBE_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_MenuGroups_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pMenuGroups);


void __RPC_STUB IAcadApplication_get_MenuGroups_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_MenuBar_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pMenuBar);


void __RPC_STUB IAcadApplication_get_MenuBar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_LoadDVB_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [in] */ BSTR Name);


void __RPC_STUB IAcadApplication_LoadDVB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_UnloadDVB_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [in] */ BSTR Name);


void __RPC_STUB IAcadApplication_UnloadDVB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_Documents_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [retval][out] */ IAcadDocuments __RPC_FAR *__RPC_FAR *pDocuments);


void __RPC_STUB IAcadApplication_get_Documents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_Eval_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [in] */ BSTR Expression);


void __RPC_STUB IAcadApplication_Eval_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_WindowState_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [retval][out] */ AcWindowState __RPC_FAR *eWinState);


void __RPC_STUB IAcadApplication_get_WindowState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_put_WindowState_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [in] */ AcWindowState eWinState);


void __RPC_STUB IAcadApplication_put_WindowState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_RunMacro_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [in] */ BSTR MacroPath);


void __RPC_STUB IAcadApplication_RunMacro_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_ZoomExtents_Proxy( 
    IAcadApplication __RPC_FAR * This);


void __RPC_STUB IAcadApplication_ZoomExtents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_ZoomAll_Proxy( 
    IAcadApplication __RPC_FAR * This);


void __RPC_STUB IAcadApplication_ZoomAll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_ZoomCenter_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [in] */ VARIANT Center,
    /* [in] */ double Magnify);


void __RPC_STUB IAcadApplication_ZoomCenter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_ZoomScaled_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [in] */ double scale,
    /* [in] */ AcZoomScaleType ScaleType);


void __RPC_STUB IAcadApplication_ZoomScaled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_ZoomWindow_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [in] */ VARIANT LowerLeft,
    /* [in] */ VARIANT UpperRight);


void __RPC_STUB IAcadApplication_ZoomWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_ZoomPickWindow_Proxy( 
    IAcadApplication __RPC_FAR * This);


void __RPC_STUB IAcadApplication_ZoomPickWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_GetAcadState_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IAcadApplication_GetAcadState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_ZoomPrevious_Proxy( 
    IAcadApplication __RPC_FAR * This);


void __RPC_STUB IAcadApplication_ZoomPrevious_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_HWND_Proxy( 
    IAcadApplication __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *HWND);


void __RPC_STUB IAcadApplication_get_HWND_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAcadApplication_INTERFACE_DEFINED__ */


#ifndef __IAcadUtility_INTERFACE_DEFINED__
#define __IAcadUtility_INTERFACE_DEFINED__

/* interface IAcadUtility */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IAcadUtility;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A6D2CC96-062C-408F-B879-550CCAA81079")
    IAcadUtility : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AngleToReal( 
            /* [in] */ BSTR Angle,
            /* [in] */ AcAngleUnits Unit,
            /* [retval][out] */ double __RPC_FAR *Value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AngleToString( 
            /* [in] */ double Angle,
            /* [in] */ AcAngleUnits Unit,
            /* [in] */ int precision,
            /* [retval][out] */ BSTR __RPC_FAR *bstrValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DistanceToReal( 
            /* [in] */ BSTR Distance,
            /* [in] */ AcUnits Unit,
            /* [retval][out] */ double __RPC_FAR *Value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RealToString( 
            /* [in] */ double Value,
            /* [in] */ AcUnits Unit,
            /* [in] */ int precision,
            /* [retval][out] */ BSTR __RPC_FAR *bstrValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TranslateCoordinates( 
            /* [in] */ VARIANT Point,
            /* [in] */ AcCoordinateSystem FromCoordSystem,
            /* [in] */ AcCoordinateSystem ToCoordSystem,
            /* [in] */ int Displacement,
            /* [optional][in] */ VARIANT OCSNormal,
            /* [retval][out] */ VARIANT __RPC_FAR *transPt) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InitializeUserInput( 
            /* [in] */ int Bits,
            /* [optional][in] */ VARIANT KeyWordList) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetInteger( 
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ int __RPC_FAR *Value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetReal( 
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ double __RPC_FAR *Value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetInput( 
            /* [retval][out] */ BSTR __RPC_FAR *Value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetKeyword( 
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ BSTR __RPC_FAR *bstrKeyword) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetString( 
            /* [in] */ int HasSpaces,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ BSTR __RPC_FAR *bstrValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAngle( 
            /* [optional][in] */ VARIANT Point,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ double __RPC_FAR *Angle) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AngleFromXAxis( 
            /* [in] */ VARIANT StartPoint,
            /* [in] */ VARIANT EndPoint,
            /* [retval][out] */ double __RPC_FAR *Angle) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCorner( 
            /* [in] */ VARIANT Point,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ VARIANT __RPC_FAR *corner) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDistance( 
            /* [optional][in] */ VARIANT Point,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ double __RPC_FAR *dist) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetOrientation( 
            /* [optional][in] */ VARIANT Point,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ double __RPC_FAR *Angle) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPoint( 
            /* [optional][in] */ VARIANT Point,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ VARIANT __RPC_FAR *inputPoint) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PolarPoint( 
            /* [in] */ VARIANT Point,
            /* [in] */ double Angle,
            /* [in] */ double Distance,
            /* [retval][out] */ VARIANT __RPC_FAR *inputPoint) = 0;
        
        virtual /* [helpstring][vararg][id] */ HRESULT STDMETHODCALLTYPE CreateTypedArray( 
            /* [out] */ VARIANT __RPC_FAR *varArr,
            /* [in] */ int Type,
            /* [in] */ SAFEARRAY __RPC_FAR * inArgs) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetEntity( 
            /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *Object,
            /* [out] */ VARIANT __RPC_FAR *PickedPoint,
            /* [optional][in] */ VARIANT Prompt) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Prompt( 
            /* [in] */ BSTR Message) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSubEntity( 
            /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *Object,
            /* [out] */ VARIANT __RPC_FAR *PickedPoint,
            /* [out] */ VARIANT __RPC_FAR *transMatrix,
            /* [out] */ VARIANT __RPC_FAR *ContextData,
            /* [optional][in] */ VARIANT Prompt) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsURL( 
            /* [in] */ BSTR URL,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *IsValidURL) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRemoteFile( 
            /* [in] */ BSTR URL,
            /* [out] */ BSTR __RPC_FAR *LocalFile,
            /* [in] */ VARIANT_BOOL IgnoreCache) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PutRemoteFile( 
            /* [in] */ BSTR URL,
            /* [in] */ BSTR LocalFile) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsRemoteFile( 
            /* [in] */ BSTR LocalFile,
            /* [out] */ BSTR __RPC_FAR *URL,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *IsDownloadedFile) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LaunchBrowserDialog( 
            /* [out] */ BSTR __RPC_FAR *SelectedURL,
            /* [in] */ BSTR DialogTitle,
            /* [in] */ BSTR OpenButtonCaption,
            /* [in] */ BSTR StartPageURL,
            /* [in] */ BSTR RegistryRootKey,
            /* [in] */ VARIANT_BOOL OpenButtonAlwaysEnabled,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *success) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAcadUtilityVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAcadUtility __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAcadUtility __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAcadUtility __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAcadUtility __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAcadUtility __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAcadUtility __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAcadUtility __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AngleToReal )( 
            IAcadUtility __RPC_FAR * This,
            /* [in] */ BSTR Angle,
            /* [in] */ AcAngleUnits Unit,
            /* [retval][out] */ double __RPC_FAR *Value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AngleToString )( 
            IAcadUtility __RPC_FAR * This,
            /* [in] */ double Angle,
            /* [in] */ AcAngleUnits Unit,
            /* [in] */ int precision,
            /* [retval][out] */ BSTR __RPC_FAR *bstrValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DistanceToReal )( 
            IAcadUtility __RPC_FAR * This,
            /* [in] */ BSTR Distance,
            /* [in] */ AcUnits Unit,
            /* [retval][out] */ double __RPC_FAR *Value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RealToString )( 
            IAcadUtility __RPC_FAR * This,
            /* [in] */ double Value,
            /* [in] */ AcUnits Unit,
            /* [in] */ int precision,
            /* [retval][out] */ BSTR __RPC_FAR *bstrValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TranslateCoordinates )( 
            IAcadUtility __RPC_FAR * This,
            /* [in] */ VARIANT Point,
            /* [in] */ AcCoordinateSystem FromCoordSystem,
            /* [in] */ AcCoordinateSystem ToCoordSystem,
            /* [in] */ int Displacement,
            /* [optional][in] */ VARIANT OCSNormal,
            /* [retval][out] */ VARIANT __RPC_FAR *transPt);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InitializeUserInput )( 
            IAcadUtility __RPC_FAR * This,
            /* [in] */ int Bits,
            /* [optional][in] */ VARIANT KeyWordList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetInteger )( 
            IAcadUtility __RPC_FAR * This,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ int __RPC_FAR *Value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetReal )( 
            IAcadUtility __RPC_FAR * This,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ double __RPC_FAR *Value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetInput )( 
            IAcadUtility __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetKeyword )( 
            IAcadUtility __RPC_FAR * This,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ BSTR __RPC_FAR *bstrKeyword);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetString )( 
            IAcadUtility __RPC_FAR * This,
            /* [in] */ int HasSpaces,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ BSTR __RPC_FAR *bstrValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAngle )( 
            IAcadUtility __RPC_FAR * This,
            /* [optional][in] */ VARIANT Point,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ double __RPC_FAR *Angle);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AngleFromXAxis )( 
            IAcadUtility __RPC_FAR * This,
            /* [in] */ VARIANT StartPoint,
            /* [in] */ VARIANT EndPoint,
            /* [retval][out] */ double __RPC_FAR *Angle);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCorner )( 
            IAcadUtility __RPC_FAR * This,
            /* [in] */ VARIANT Point,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ VARIANT __RPC_FAR *corner);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDistance )( 
            IAcadUtility __RPC_FAR * This,
            /* [optional][in] */ VARIANT Point,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ double __RPC_FAR *dist);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetOrientation )( 
            IAcadUtility __RPC_FAR * This,
            /* [optional][in] */ VARIANT Point,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ double __RPC_FAR *Angle);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPoint )( 
            IAcadUtility __RPC_FAR * This,
            /* [optional][in] */ VARIANT Point,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ VARIANT __RPC_FAR *inputPoint);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PolarPoint )( 
            IAcadUtility __RPC_FAR * This,
            /* [in] */ VARIANT Point,
            /* [in] */ double Angle,
            /* [in] */ double Distance,
            /* [retval][out] */ VARIANT __RPC_FAR *inputPoint);
        
        /* [helpstring][vararg][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateTypedArray )( 
            IAcadUtility __RPC_FAR * This,
            /* [out] */ VARIANT __RPC_FAR *varArr,
            /* [in] */ int Type,
            /* [in] */ SAFEARRAY __RPC_FAR * inArgs);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEntity )( 
            IAcadUtility __RPC_FAR * This,
            /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *Object,
            /* [out] */ VARIANT __RPC_FAR *PickedPoint,
            /* [optional][in] */ VARIANT Prompt);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Prompt )( 
            IAcadUtility __RPC_FAR * This,
            /* [in] */ BSTR Message);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSubEntity )( 
            IAcadUtility __RPC_FAR * This,
            /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *Object,
            /* [out] */ VARIANT __RPC_FAR *PickedPoint,
            /* [out] */ VARIANT __RPC_FAR *transMatrix,
            /* [out] */ VARIANT __RPC_FAR *ContextData,
            /* [optional][in] */ VARIANT Prompt);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsURL )( 
            IAcadUtility __RPC_FAR * This,
            /* [in] */ BSTR URL,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *IsValidURL);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRemoteFile )( 
            IAcadUtility __RPC_FAR * This,
            /* [in] */ BSTR URL,
            /* [out] */ BSTR __RPC_FAR *LocalFile,
            /* [in] */ VARIANT_BOOL IgnoreCache);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PutRemoteFile )( 
            IAcadUtility __RPC_FAR * This,
            /* [in] */ BSTR URL,
            /* [in] */ BSTR LocalFile);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsRemoteFile )( 
            IAcadUtility __RPC_FAR * This,
            /* [in] */ BSTR LocalFile,
            /* [out] */ BSTR __RPC_FAR *URL,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *IsDownloadedFile);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LaunchBrowserDialog )( 
            IAcadUtility __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *SelectedURL,
            /* [in] */ BSTR DialogTitle,
            /* [in] */ BSTR OpenButtonCaption,
            /* [in] */ BSTR StartPageURL,
            /* [in] */ BSTR RegistryRootKey,
            /* [in] */ VARIANT_BOOL OpenButtonAlwaysEnabled,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *success);
        
        END_INTERFACE
    } IAcadUtilityVtbl;

    interface IAcadUtility
    {
        CONST_VTBL struct IAcadUtilityVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAcadUtility_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAcadUtility_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAcadUtility_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAcadUtility_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAcadUtility_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAcadUtility_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAcadUtility_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAcadUtility_AngleToReal(This,Angle,Unit,Value)	\
    (This)->lpVtbl -> AngleToReal(This,Angle,Unit,Value)

#define IAcadUtility_AngleToString(This,Angle,Unit,precision,bstrValue)	\
    (This)->lpVtbl -> AngleToString(This,Angle,Unit,precision,bstrValue)

#define IAcadUtility_DistanceToReal(This,Distance,Unit,Value)	\
    (This)->lpVtbl -> DistanceToReal(This,Distance,Unit,Value)

#define IAcadUtility_RealToString(This,Value,Unit,precision,bstrValue)	\
    (This)->lpVtbl -> RealToString(This,Value,Unit,precision,bstrValue)

#define IAcadUtility_TranslateCoordinates(This,Point,FromCoordSystem,ToCoordSystem,Displacement,OCSNormal,transPt)	\
    (This)->lpVtbl -> TranslateCoordinates(This,Point,FromCoordSystem,ToCoordSystem,Displacement,OCSNormal,transPt)

#define IAcadUtility_InitializeUserInput(This,Bits,KeyWordList)	\
    (This)->lpVtbl -> InitializeUserInput(This,Bits,KeyWordList)

#define IAcadUtility_GetInteger(This,Prompt,Value)	\
    (This)->lpVtbl -> GetInteger(This,Prompt,Value)

#define IAcadUtility_GetReal(This,Prompt,Value)	\
    (This)->lpVtbl -> GetReal(This,Prompt,Value)

#define IAcadUtility_GetInput(This,Value)	\
    (This)->lpVtbl -> GetInput(This,Value)

#define IAcadUtility_GetKeyword(This,Prompt,bstrKeyword)	\
    (This)->lpVtbl -> GetKeyword(This,Prompt,bstrKeyword)

#define IAcadUtility_GetString(This,HasSpaces,Prompt,bstrValue)	\
    (This)->lpVtbl -> GetString(This,HasSpaces,Prompt,bstrValue)

#define IAcadUtility_GetAngle(This,Point,Prompt,Angle)	\
    (This)->lpVtbl -> GetAngle(This,Point,Prompt,Angle)

#define IAcadUtility_AngleFromXAxis(This,StartPoint,EndPoint,Angle)	\
    (This)->lpVtbl -> AngleFromXAxis(This,StartPoint,EndPoint,Angle)

#define IAcadUtility_GetCorner(This,Point,Prompt,corner)	\
    (This)->lpVtbl -> GetCorner(This,Point,Prompt,corner)

#define IAcadUtility_GetDistance(This,Point,Prompt,dist)	\
    (This)->lpVtbl -> GetDistance(This,Point,Prompt,dist)

#define IAcadUtility_GetOrientation(This,Point,Prompt,Angle)	\
    (This)->lpVtbl -> GetOrientation(This,Point,Prompt,Angle)

#define IAcadUtility_GetPoint(This,Point,Prompt,inputPoint)	\
    (This)->lpVtbl -> GetPoint(This,Point,Prompt,inputPoint)

#define IAcadUtility_PolarPoint(This,Point,Angle,Distance,inputPoint)	\
    (This)->lpVtbl -> PolarPoint(This,Point,Angle,Distance,inputPoint)

#define IAcadUtility_CreateTypedArray(This,varArr,Type,inArgs)	\
    (This)->lpVtbl -> CreateTypedArray(This,varArr,Type,inArgs)

#define IAcadUtility_GetEntity(This,Object,PickedPoint,Prompt)	\
    (This)->lpVtbl -> GetEntity(This,Object,PickedPoint,Prompt)

#define IAcadUtility_Prompt(This,Message)	\
    (This)->lpVtbl -> Prompt(This,Message)

#define IAcadUtility_GetSubEntity(This,Object,PickedPoint,transMatrix,ContextData,Prompt)	\
    (This)->lpVtbl -> GetSubEntity(This,Object,PickedPoint,transMatrix,ContextData,Prompt)

#define IAcadUtility_IsURL(This,URL,IsValidURL)	\
    (This)->lpVtbl -> IsURL(This,URL,IsValidURL)

#define IAcadUtility_GetRemoteFile(This,URL,LocalFile,IgnoreCache)	\
    (This)->lpVtbl -> GetRemoteFile(This,URL,LocalFile,IgnoreCache)

#define IAcadUtility_PutRemoteFile(This,URL,LocalFile)	\
    (This)->lpVtbl -> PutRemoteFile(This,URL,LocalFile)

#define IAcadUtility_IsRemoteFile(This,LocalFile,URL,IsDownloadedFile)	\
    (This)->lpVtbl -> IsRemoteFile(This,LocalFile,URL,IsDownloadedFile)

#define IAcadUtility_LaunchBrowserDialog(This,SelectedURL,DialogTitle,OpenButtonCaption,StartPageURL,RegistryRootKey,OpenButtonAlwaysEnabled,success)	\
    (This)->lpVtbl -> LaunchBrowserDialog(This,SelectedURL,DialogTitle,OpenButtonCaption,StartPageURL,RegistryRootKey,OpenButtonAlwaysEnabled,success)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_AngleToReal_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [in] */ BSTR Angle,
    /* [in] */ AcAngleUnits Unit,
    /* [retval][out] */ double __RPC_FAR *Value);


void __RPC_STUB IAcadUtility_AngleToReal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_AngleToString_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [in] */ double Angle,
    /* [in] */ AcAngleUnits Unit,
    /* [in] */ int precision,
    /* [retval][out] */ BSTR __RPC_FAR *bstrValue);


void __RPC_STUB IAcadUtility_AngleToString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_DistanceToReal_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [in] */ BSTR Distance,
    /* [in] */ AcUnits Unit,
    /* [retval][out] */ double __RPC_FAR *Value);


void __RPC_STUB IAcadUtility_DistanceToReal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_RealToString_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [in] */ double Value,
    /* [in] */ AcUnits Unit,
    /* [in] */ int precision,
    /* [retval][out] */ BSTR __RPC_FAR *bstrValue);


void __RPC_STUB IAcadUtility_RealToString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_TranslateCoordinates_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [in] */ VARIANT Point,
    /* [in] */ AcCoordinateSystem FromCoordSystem,
    /* [in] */ AcCoordinateSystem ToCoordSystem,
    /* [in] */ int Displacement,
    /* [optional][in] */ VARIANT OCSNormal,
    /* [retval][out] */ VARIANT __RPC_FAR *transPt);


void __RPC_STUB IAcadUtility_TranslateCoordinates_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_InitializeUserInput_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [in] */ int Bits,
    /* [optional][in] */ VARIANT KeyWordList);


void __RPC_STUB IAcadUtility_InitializeUserInput_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetInteger_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [optional][in] */ VARIANT Prompt,
    /* [retval][out] */ int __RPC_FAR *Value);


void __RPC_STUB IAcadUtility_GetInteger_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetReal_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [optional][in] */ VARIANT Prompt,
    /* [retval][out] */ double __RPC_FAR *Value);


void __RPC_STUB IAcadUtility_GetReal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetInput_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Value);


void __RPC_STUB IAcadUtility_GetInput_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetKeyword_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [optional][in] */ VARIANT Prompt,
    /* [retval][out] */ BSTR __RPC_FAR *bstrKeyword);


void __RPC_STUB IAcadUtility_GetKeyword_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetString_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [in] */ int HasSpaces,
    /* [optional][in] */ VARIANT Prompt,
    /* [retval][out] */ BSTR __RPC_FAR *bstrValue);


void __RPC_STUB IAcadUtility_GetString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetAngle_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [optional][in] */ VARIANT Point,
    /* [optional][in] */ VARIANT Prompt,
    /* [retval][out] */ double __RPC_FAR *Angle);


void __RPC_STUB IAcadUtility_GetAngle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_AngleFromXAxis_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [in] */ VARIANT StartPoint,
    /* [in] */ VARIANT EndPoint,
    /* [retval][out] */ double __RPC_FAR *Angle);


void __RPC_STUB IAcadUtility_AngleFromXAxis_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetCorner_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [in] */ VARIANT Point,
    /* [optional][in] */ VARIANT Prompt,
    /* [retval][out] */ VARIANT __RPC_FAR *corner);


void __RPC_STUB IAcadUtility_GetCorner_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetDistance_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [optional][in] */ VARIANT Point,
    /* [optional][in] */ VARIANT Prompt,
    /* [retval][out] */ double __RPC_FAR *dist);


void __RPC_STUB IAcadUtility_GetDistance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetOrientation_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [optional][in] */ VARIANT Point,
    /* [optional][in] */ VARIANT Prompt,
    /* [retval][out] */ double __RPC_FAR *Angle);


void __RPC_STUB IAcadUtility_GetOrientation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetPoint_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [optional][in] */ VARIANT Point,
    /* [optional][in] */ VARIANT Prompt,
    /* [retval][out] */ VARIANT __RPC_FAR *inputPoint);


void __RPC_STUB IAcadUtility_GetPoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_PolarPoint_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [in] */ VARIANT Point,
    /* [in] */ double Angle,
    /* [in] */ double Distance,
    /* [retval][out] */ VARIANT __RPC_FAR *inputPoint);


void __RPC_STUB IAcadUtility_PolarPoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][vararg][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_CreateTypedArray_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [out] */ VARIANT __RPC_FAR *varArr,
    /* [in] */ int Type,
    /* [in] */ SAFEARRAY __RPC_FAR * inArgs);


void __RPC_STUB IAcadUtility_CreateTypedArray_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetEntity_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *Object,
    /* [out] */ VARIANT __RPC_FAR *PickedPoint,
    /* [optional][in] */ VARIANT Prompt);


void __RPC_STUB IAcadUtility_GetEntity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_Prompt_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [in] */ BSTR Message);


void __RPC_STUB IAcadUtility_Prompt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetSubEntity_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *Object,
    /* [out] */ VARIANT __RPC_FAR *PickedPoint,
    /* [out] */ VARIANT __RPC_FAR *transMatrix,
    /* [out] */ VARIANT __RPC_FAR *ContextData,
    /* [optional][in] */ VARIANT Prompt);


void __RPC_STUB IAcadUtility_GetSubEntity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_IsURL_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [in] */ BSTR URL,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *IsValidURL);


void __RPC_STUB IAcadUtility_IsURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetRemoteFile_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [in] */ BSTR URL,
    /* [out] */ BSTR __RPC_FAR *LocalFile,
    /* [in] */ VARIANT_BOOL IgnoreCache);


void __RPC_STUB IAcadUtility_GetRemoteFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_PutRemoteFile_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [in] */ BSTR URL,
    /* [in] */ BSTR LocalFile);


void __RPC_STUB IAcadUtility_PutRemoteFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_IsRemoteFile_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [in] */ BSTR LocalFile,
    /* [out] */ BSTR __RPC_FAR *URL,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *IsDownloadedFile);


void __RPC_STUB IAcadUtility_IsRemoteFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_LaunchBrowserDialog_Proxy( 
    IAcadUtility __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *SelectedURL,
    /* [in] */ BSTR DialogTitle,
    /* [in] */ BSTR OpenButtonCaption,
    /* [in] */ BSTR StartPageURL,
    /* [in] */ BSTR RegistryRootKey,
    /* [in] */ VARIANT_BOOL OpenButtonAlwaysEnabled,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *success);


void __RPC_STUB IAcadUtility_LaunchBrowserDialog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAcadUtility_INTERFACE_DEFINED__ */


#ifndef __IAcadDocument_INTERFACE_DEFINED__
#define __IAcadDocument_INTERFACE_DEFINED__

/* interface IAcadDocument */
/* [oleautomation][dual][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IAcadDocument;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3C391D99-2886-4E1A-9855-EE30C9FC5B8A")
    IAcadDocument : public IAcadDatabase
    {
    public:
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Plot( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pPlot) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActiveLayer( 
            /* [retval][out] */ IAcadLayer __RPC_FAR *__RPC_FAR *pActLayer) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ActiveLayer( 
            /* [in] */ IAcadLayer __RPC_FAR *pActLayer) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActiveLinetype( 
            /* [retval][out] */ IAcadLineType __RPC_FAR *__RPC_FAR *pActLinetype) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ActiveLinetype( 
            /* [in] */ IAcadLineType __RPC_FAR *pActLinetype) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActiveDimStyle( 
            /* [retval][out] */ IAcadDimStyle __RPC_FAR *__RPC_FAR *pActDimStyle) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ActiveDimStyle( 
            /* [in] */ IAcadDimStyle __RPC_FAR *pActDimStyle) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActiveTextStyle( 
            /* [retval][out] */ IAcadTextStyle __RPC_FAR *__RPC_FAR *pActTextStyle) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ActiveTextStyle( 
            /* [in] */ IAcadTextStyle __RPC_FAR *pActTextStyle) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActiveUCS( 
            /* [retval][out] */ IAcadUCS __RPC_FAR *__RPC_FAR *pActUCS) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ActiveUCS( 
            /* [in] */ IAcadUCS __RPC_FAR *pActUCS) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActiveViewport( 
            /* [retval][out] */ IAcadViewport __RPC_FAR *__RPC_FAR *pActView) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ActiveViewport( 
            /* [in] */ IAcadViewport __RPC_FAR *pActView) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActivePViewport( 
            /* [retval][out] */ IAcadPViewport __RPC_FAR *__RPC_FAR *pActView) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ActivePViewport( 
            /* [in] */ IAcadPViewport __RPC_FAR *pActView) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActiveSpace( 
            /* [retval][out] */ AcActiveSpace __RPC_FAR *ActSpace) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ActiveSpace( 
            /* [in] */ AcActiveSpace ActSpace) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_SelectionSets( 
            /* [retval][out] */ IAcadSelectionSets __RPC_FAR *__RPC_FAR *pSelSets) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActiveSelectionSet( 
            /* [retval][out] */ IAcadSelectionSet __RPC_FAR *__RPC_FAR *pSelSet) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_FullName( 
            /* [retval][out] */ BSTR __RPC_FAR *FullName) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *Name) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Path( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ObjectSnapMode( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *fSnapMode) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ObjectSnapMode( 
            /* [in] */ VARIANT_BOOL fSnapMode) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ReadOnly( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *bReadOnly) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Saved( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *bSaved) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_MSpace( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Mode) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_MSpace( 
            /* [in] */ VARIANT_BOOL Mode) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Utility( 
            /* [retval][out] */ IAcadUtility __RPC_FAR *__RPC_FAR *pUtil) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Open( 
            /* [in] */ BSTR FullName,
            /* [optional][in] */ VARIANT Password,
            /* [retval][out] */ IAcadDocument __RPC_FAR *__RPC_FAR *pDocObj) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AuditInfo( 
            /* [in] */ VARIANT_BOOL FixErr) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Import( 
            /* [in] */ BSTR FileName,
            /* [in] */ VARIANT InsertionPoint,
            /* [in] */ double ScaleFactor,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pObj) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Export( 
            /* [in] */ BSTR FileName,
            /* [in] */ BSTR Extension,
            /* [in] */ IAcadSelectionSet __RPC_FAR *SelectionSet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE New( 
            /* [in] */ BSTR TemplateFileName,
            /* [retval][out] */ IAcadDocument __RPC_FAR *__RPC_FAR *pDocObj) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveAs( 
            /* [in] */ BSTR FullFileName,
            /* [optional][in] */ VARIANT SaveAsType,
            /* [optional][in] */ VARIANT vSecurityParams) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Wblock( 
            /* [in] */ BSTR FileName,
            /* [in] */ IAcadSelectionSet __RPC_FAR *SelectionSet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PurgeAll( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetVariable( 
            /* [in] */ BSTR Name,
            /* [retval][out] */ VARIANT __RPC_FAR *Value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetVariable( 
            /* [in] */ BSTR Name,
            /* [in] */ VARIANT Value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadShapeFile( 
            /* [in] */ BSTR FullName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Regen( 
            /* [in] */ AcRegenType WhichViewports) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_PickfirstSelectionSet( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pSelSet) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Active( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pvbActive) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Activate( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Close( 
            /* [optional][in] */ VARIANT SaveChanges,
            /* [optional][in] */ VARIANT FileName) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_WindowState( 
            /* [in] */ AcWindowState pWinState) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_WindowState( 
            /* [retval][out] */ AcWindowState __RPC_FAR *pWinState) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Width( 
            /* [in] */ int pWidth) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Width( 
            /* [retval][out] */ int __RPC_FAR *pWidth) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Height( 
            /* [in] */ int pHeight) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Height( 
            /* [retval][out] */ int __RPC_FAR *pHeight) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ActiveLayout( 
            /* [in] */ IAcadLayout __RPC_FAR *pLayout) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActiveLayout( 
            /* [retval][out] */ IAcadLayout __RPC_FAR *__RPC_FAR *pLayout) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SendCommand( 
            /* [in] */ BSTR Command) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_HWND( 
            /* [retval][out] */ long __RPC_FAR *HWND) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_WindowTitle( 
            /* [retval][out] */ BSTR __RPC_FAR *Title) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IAcadApplication __RPC_FAR *__RPC_FAR *pAppObj) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Database( 
            /* [retval][out] */ IAcadDatabase __RPC_FAR *__RPC_FAR *pDatabase) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StartUndoMark( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EndUndoMark( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAcadDocumentVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAcadDocument __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAcadDocument __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAcadDocument __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ModelSpace )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadModelSpace __RPC_FAR *__RPC_FAR *pMSpace);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PaperSpace )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadPaperSpace __RPC_FAR *__RPC_FAR *pPSpace);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Blocks )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadBlocks __RPC_FAR *__RPC_FAR *pBlocks);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CopyObjects )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ VARIANT Objects,
            /* [optional][in] */ VARIANT Owner,
            /* [optional][out][in] */ VARIANT __RPC_FAR *IdPairs,
            /* [retval][out] */ VARIANT __RPC_FAR *pNewObjects);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Groups )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadGroups __RPC_FAR *__RPC_FAR *pGroups);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DimStyles )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadDimStyles __RPC_FAR *__RPC_FAR *pDimStyles);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Layers )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadLayers __RPC_FAR *__RPC_FAR *pLayers);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Linetypes )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadLineTypes __RPC_FAR *__RPC_FAR *pLinetypes);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Dictionaries )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadDictionaries __RPC_FAR *__RPC_FAR *pDictionaries);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RegisteredApplications )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadRegisteredApplications __RPC_FAR *__RPC_FAR *pRegApps);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TextStyles )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadTextStyles __RPC_FAR *__RPC_FAR *pTextStyles);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UserCoordinateSystems )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadUCSs __RPC_FAR *__RPC_FAR *pUCSs);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Views )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadViews __RPC_FAR *__RPC_FAR *pViews);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Viewports )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadViewports __RPC_FAR *__RPC_FAR *pViewports);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ElevationModelSpace )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *Elevation);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ElevationModelSpace )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ double Elevation);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ElevationPaperSpace )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *Elevation);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ElevationPaperSpace )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ double Elevation);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Limits )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *Limits);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Limits )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ VARIANT Limits);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *HandleToObject )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ BSTR Handle,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pObj);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ObjectIdToObject )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ long ObjectID,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pObj);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Layouts )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadLayouts __RPC_FAR *__RPC_FAR *pLayouts);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlotConfigurations )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadPlotConfigurations __RPC_FAR *__RPC_FAR *pPlotConfigs);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Preferences )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadDatabasePreferences __RPC_FAR *__RPC_FAR *pPref);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FileDependencies )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadFileDependencies __RPC_FAR *__RPC_FAR *pFDM);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SummaryInfo )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadSummaryInfo __RPC_FAR *__RPC_FAR *pSummaryInfo);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Plot )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pPlot);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveLayer )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadLayer __RPC_FAR *__RPC_FAR *pActLayer);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ActiveLayer )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ IAcadLayer __RPC_FAR *pActLayer);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveLinetype )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadLineType __RPC_FAR *__RPC_FAR *pActLinetype);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ActiveLinetype )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ IAcadLineType __RPC_FAR *pActLinetype);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveDimStyle )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadDimStyle __RPC_FAR *__RPC_FAR *pActDimStyle);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ActiveDimStyle )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ IAcadDimStyle __RPC_FAR *pActDimStyle);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveTextStyle )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadTextStyle __RPC_FAR *__RPC_FAR *pActTextStyle);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ActiveTextStyle )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ IAcadTextStyle __RPC_FAR *pActTextStyle);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveUCS )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadUCS __RPC_FAR *__RPC_FAR *pActUCS);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ActiveUCS )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ IAcadUCS __RPC_FAR *pActUCS);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveViewport )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadViewport __RPC_FAR *__RPC_FAR *pActView);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ActiveViewport )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ IAcadViewport __RPC_FAR *pActView);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActivePViewport )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadPViewport __RPC_FAR *__RPC_FAR *pActView);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ActivePViewport )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ IAcadPViewport __RPC_FAR *pActView);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveSpace )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ AcActiveSpace __RPC_FAR *ActSpace);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ActiveSpace )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ AcActiveSpace ActSpace);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SelectionSets )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadSelectionSets __RPC_FAR *__RPC_FAR *pSelSets);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveSelectionSet )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadSelectionSet __RPC_FAR *__RPC_FAR *pSelSet);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FullName )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *FullName);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Name);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Path )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ObjectSnapMode )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *fSnapMode);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ObjectSnapMode )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL fSnapMode);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReadOnly )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *bReadOnly);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Saved )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *bSaved);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MSpace )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Mode);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MSpace )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Mode);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Utility )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadUtility __RPC_FAR *__RPC_FAR *pUtil);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Open )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ BSTR FullName,
            /* [optional][in] */ VARIANT Password,
            /* [retval][out] */ IAcadDocument __RPC_FAR *__RPC_FAR *pDocObj);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AuditInfo )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL FixErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Import )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ BSTR FileName,
            /* [in] */ VARIANT InsertionPoint,
            /* [in] */ double ScaleFactor,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pObj);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Export )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ BSTR FileName,
            /* [in] */ BSTR Extension,
            /* [in] */ IAcadSelectionSet __RPC_FAR *SelectionSet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *New )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ BSTR TemplateFileName,
            /* [retval][out] */ IAcadDocument __RPC_FAR *__RPC_FAR *pDocObj);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            IAcadDocument __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveAs )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ BSTR FullFileName,
            /* [optional][in] */ VARIANT SaveAsType,
            /* [optional][in] */ VARIANT vSecurityParams);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Wblock )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ BSTR FileName,
            /* [in] */ IAcadSelectionSet __RPC_FAR *SelectionSet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PurgeAll )( 
            IAcadDocument __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetVariable )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [retval][out] */ VARIANT __RPC_FAR *Value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetVariable )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [in] */ VARIANT Value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LoadShapeFile )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ BSTR FullName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Regen )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ AcRegenType WhichViewports);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PickfirstSelectionSet )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pSelSet);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Active )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pvbActive);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Activate )( 
            IAcadDocument __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Close )( 
            IAcadDocument __RPC_FAR * This,
            /* [optional][in] */ VARIANT SaveChanges,
            /* [optional][in] */ VARIANT FileName);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WindowState )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ AcWindowState pWinState);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WindowState )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ AcWindowState __RPC_FAR *pWinState);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Width )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ int pWidth);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Width )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *pWidth);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Height )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ int pHeight);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Height )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *pHeight);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ActiveLayout )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ IAcadLayout __RPC_FAR *pLayout);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveLayout )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadLayout __RPC_FAR *__RPC_FAR *pLayout);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SendCommand )( 
            IAcadDocument __RPC_FAR * This,
            /* [in] */ BSTR Command);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HWND )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *HWND);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WindowTitle )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Title);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Application )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadApplication __RPC_FAR *__RPC_FAR *pAppObj);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Database )( 
            IAcadDocument __RPC_FAR * This,
            /* [retval][out] */ IAcadDatabase __RPC_FAR *__RPC_FAR *pDatabase);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StartUndoMark )( 
            IAcadDocument __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EndUndoMark )( 
            IAcadDocument __RPC_FAR * This);
        
        END_INTERFACE
    } IAcadDocumentVtbl;

    interface IAcadDocument
    {
        CONST_VTBL struct IAcadDocumentVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAcadDocument_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAcadDocument_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAcadDocument_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAcadDocument_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAcadDocument_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAcadDocument_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAcadDocument_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAcadDocument_get_ModelSpace(This,pMSpace)	\
    (This)->lpVtbl -> get_ModelSpace(This,pMSpace)

#define IAcadDocument_get_PaperSpace(This,pPSpace)	\
    (This)->lpVtbl -> get_PaperSpace(This,pPSpace)

#define IAcadDocument_get_Blocks(This,pBlocks)	\
    (This)->lpVtbl -> get_Blocks(This,pBlocks)

#define IAcadDocument_CopyObjects(This,Objects,Owner,IdPairs,pNewObjects)	\
    (This)->lpVtbl -> CopyObjects(This,Objects,Owner,IdPairs,pNewObjects)

#define IAcadDocument_get_Groups(This,pGroups)	\
    (This)->lpVtbl -> get_Groups(This,pGroups)

#define IAcadDocument_get_DimStyles(This,pDimStyles)	\
    (This)->lpVtbl -> get_DimStyles(This,pDimStyles)

#define IAcadDocument_get_Layers(This,pLayers)	\
    (This)->lpVtbl -> get_Layers(This,pLayers)

#define IAcadDocument_get_Linetypes(This,pLinetypes)	\
    (This)->lpVtbl -> get_Linetypes(This,pLinetypes)

#define IAcadDocument_get_Dictionaries(This,pDictionaries)	\
    (This)->lpVtbl -> get_Dictionaries(This,pDictionaries)

#define IAcadDocument_get_RegisteredApplications(This,pRegApps)	\
    (This)->lpVtbl -> get_RegisteredApplications(This,pRegApps)

#define IAcadDocument_get_TextStyles(This,pTextStyles)	\
    (This)->lpVtbl -> get_TextStyles(This,pTextStyles)

#define IAcadDocument_get_UserCoordinateSystems(This,pUCSs)	\
    (This)->lpVtbl -> get_UserCoordinateSystems(This,pUCSs)

#define IAcadDocument_get_Views(This,pViews)	\
    (This)->lpVtbl -> get_Views(This,pViews)

#define IAcadDocument_get_Viewports(This,pViewports)	\
    (This)->lpVtbl -> get_Viewports(This,pViewports)

#define IAcadDocument_get_ElevationModelSpace(This,Elevation)	\
    (This)->lpVtbl -> get_ElevationModelSpace(This,Elevation)

#define IAcadDocument_put_ElevationModelSpace(This,Elevation)	\
    (This)->lpVtbl -> put_ElevationModelSpace(This,Elevation)

#define IAcadDocument_get_ElevationPaperSpace(This,Elevation)	\
    (This)->lpVtbl -> get_ElevationPaperSpace(This,Elevation)

#define IAcadDocument_put_ElevationPaperSpace(This,Elevation)	\
    (This)->lpVtbl -> put_ElevationPaperSpace(This,Elevation)

#define IAcadDocument_get_Limits(This,Limits)	\
    (This)->lpVtbl -> get_Limits(This,Limits)

#define IAcadDocument_put_Limits(This,Limits)	\
    (This)->lpVtbl -> put_Limits(This,Limits)

#define IAcadDocument_HandleToObject(This,Handle,pObj)	\
    (This)->lpVtbl -> HandleToObject(This,Handle,pObj)

#define IAcadDocument_ObjectIdToObject(This,ObjectID,pObj)	\
    (This)->lpVtbl -> ObjectIdToObject(This,ObjectID,pObj)

#define IAcadDocument_get_Layouts(This,pLayouts)	\
    (This)->lpVtbl -> get_Layouts(This,pLayouts)

#define IAcadDocument_get_PlotConfigurations(This,pPlotConfigs)	\
    (This)->lpVtbl -> get_PlotConfigurations(This,pPlotConfigs)

#define IAcadDocument_get_Preferences(This,pPref)	\
    (This)->lpVtbl -> get_Preferences(This,pPref)

#define IAcadDocument_get_FileDependencies(This,pFDM)	\
    (This)->lpVtbl -> get_FileDependencies(This,pFDM)

#define IAcadDocument_get_SummaryInfo(This,pSummaryInfo)	\
    (This)->lpVtbl -> get_SummaryInfo(This,pSummaryInfo)


#define IAcadDocument_get_Plot(This,pPlot)	\
    (This)->lpVtbl -> get_Plot(This,pPlot)

#define IAcadDocument_get_ActiveLayer(This,pActLayer)	\
    (This)->lpVtbl -> get_ActiveLayer(This,pActLayer)

#define IAcadDocument_put_ActiveLayer(This,pActLayer)	\
    (This)->lpVtbl -> put_ActiveLayer(This,pActLayer)

#define IAcadDocument_get_ActiveLinetype(This,pActLinetype)	\
    (This)->lpVtbl -> get_ActiveLinetype(This,pActLinetype)

#define IAcadDocument_put_ActiveLinetype(This,pActLinetype)	\
    (This)->lpVtbl -> put_ActiveLinetype(This,pActLinetype)

#define IAcadDocument_get_ActiveDimStyle(This,pActDimStyle)	\
    (This)->lpVtbl -> get_ActiveDimStyle(This,pActDimStyle)

#define IAcadDocument_put_ActiveDimStyle(This,pActDimStyle)	\
    (This)->lpVtbl -> put_ActiveDimStyle(This,pActDimStyle)

#define IAcadDocument_get_ActiveTextStyle(This,pActTextStyle)	\
    (This)->lpVtbl -> get_ActiveTextStyle(This,pActTextStyle)

#define IAcadDocument_put_ActiveTextStyle(This,pActTextStyle)	\
    (This)->lpVtbl -> put_ActiveTextStyle(This,pActTextStyle)

#define IAcadDocument_get_ActiveUCS(This,pActUCS)	\
    (This)->lpVtbl -> get_ActiveUCS(This,pActUCS)

#define IAcadDocument_put_ActiveUCS(This,pActUCS)	\
    (This)->lpVtbl -> put_ActiveUCS(This,pActUCS)

#define IAcadDocument_get_ActiveViewport(This,pActView)	\
    (This)->lpVtbl -> get_ActiveViewport(This,pActView)

#define IAcadDocument_put_ActiveViewport(This,pActView)	\
    (This)->lpVtbl -> put_ActiveViewport(This,pActView)

#define IAcadDocument_get_ActivePViewport(This,pActView)	\
    (This)->lpVtbl -> get_ActivePViewport(This,pActView)

#define IAcadDocument_put_ActivePViewport(This,pActView)	\
    (This)->lpVtbl -> put_ActivePViewport(This,pActView)

#define IAcadDocument_get_ActiveSpace(This,ActSpace)	\
    (This)->lpVtbl -> get_ActiveSpace(This,ActSpace)

#define IAcadDocument_put_ActiveSpace(This,ActSpace)	\
    (This)->lpVtbl -> put_ActiveSpace(This,ActSpace)

#define IAcadDocument_get_SelectionSets(This,pSelSets)	\
    (This)->lpVtbl -> get_SelectionSets(This,pSelSets)

#define IAcadDocument_get_ActiveSelectionSet(This,pSelSet)	\
    (This)->lpVtbl -> get_ActiveSelectionSet(This,pSelSet)

#define IAcadDocument_get_FullName(This,FullName)	\
    (This)->lpVtbl -> get_FullName(This,FullName)

#define IAcadDocument_get_Name(This,Name)	\
    (This)->lpVtbl -> get_Name(This,Name)

#define IAcadDocument_get_Path(This,Path)	\
    (This)->lpVtbl -> get_Path(This,Path)

#define IAcadDocument_get_ObjectSnapMode(This,fSnapMode)	\
    (This)->lpVtbl -> get_ObjectSnapMode(This,fSnapMode)

#define IAcadDocument_put_ObjectSnapMode(This,fSnapMode)	\
    (This)->lpVtbl -> put_ObjectSnapMode(This,fSnapMode)

#define IAcadDocument_get_ReadOnly(This,bReadOnly)	\
    (This)->lpVtbl -> get_ReadOnly(This,bReadOnly)

#define IAcadDocument_get_Saved(This,bSaved)	\
    (This)->lpVtbl -> get_Saved(This,bSaved)

#define IAcadDocument_get_MSpace(This,Mode)	\
    (This)->lpVtbl -> get_MSpace(This,Mode)

#define IAcadDocument_put_MSpace(This,Mode)	\
    (This)->lpVtbl -> put_MSpace(This,Mode)

#define IAcadDocument_get_Utility(This,pUtil)	\
    (This)->lpVtbl -> get_Utility(This,pUtil)

#define IAcadDocument_Open(This,FullName,Password,pDocObj)	\
    (This)->lpVtbl -> Open(This,FullName,Password,pDocObj)

#define IAcadDocument_AuditInfo(This,FixErr)	\
    (This)->lpVtbl -> AuditInfo(This,FixErr)

#define IAcadDocument_Import(This,FileName,InsertionPoint,ScaleFactor,pObj)	\
    (This)->lpVtbl -> Import(This,FileName,InsertionPoint,ScaleFactor,pObj)

#define IAcadDocument_Export(This,FileName,Extension,SelectionSet)	\
    (This)->lpVtbl -> Export(This,FileName,Extension,SelectionSet)

#define IAcadDocument_New(This,TemplateFileName,pDocObj)	\
    (This)->lpVtbl -> New(This,TemplateFileName,pDocObj)

#define IAcadDocument_Save(This)	\
    (This)->lpVtbl -> Save(This)

#define IAcadDocument_SaveAs(This,FullFileName,SaveAsType,vSecurityParams)	\
    (This)->lpVtbl -> SaveAs(This,FullFileName,SaveAsType,vSecurityParams)

#define IAcadDocument_Wblock(This,FileName,SelectionSet)	\
    (This)->lpVtbl -> Wblock(This,FileName,SelectionSet)

#define IAcadDocument_PurgeAll(This)	\
    (This)->lpVtbl -> PurgeAll(This)

#define IAcadDocument_GetVariable(This,Name,Value)	\
    (This)->lpVtbl -> GetVariable(This,Name,Value)

#define IAcadDocument_SetVariable(This,Name,Value)	\
    (This)->lpVtbl -> SetVariable(This,Name,Value)

#define IAcadDocument_LoadShapeFile(This,FullName)	\
    (This)->lpVtbl -> LoadShapeFile(This,FullName)

#define IAcadDocument_Regen(This,WhichViewports)	\
    (This)->lpVtbl -> Regen(This,WhichViewports)

#define IAcadDocument_get_PickfirstSelectionSet(This,pSelSet)	\
    (This)->lpVtbl -> get_PickfirstSelectionSet(This,pSelSet)

#define IAcadDocument_get_Active(This,pvbActive)	\
    (This)->lpVtbl -> get_Active(This,pvbActive)

#define IAcadDocument_Activate(This)	\
    (This)->lpVtbl -> Activate(This)

#define IAcadDocument_Close(This,SaveChanges,FileName)	\
    (This)->lpVtbl -> Close(This,SaveChanges,FileName)

#define IAcadDocument_put_WindowState(This,pWinState)	\
    (This)->lpVtbl -> put_WindowState(This,pWinState)

#define IAcadDocument_get_WindowState(This,pWinState)	\
    (This)->lpVtbl -> get_WindowState(This,pWinState)

#define IAcadDocument_put_Width(This,pWidth)	\
    (This)->lpVtbl -> put_Width(This,pWidth)

#define IAcadDocument_get_Width(This,pWidth)	\
    (This)->lpVtbl -> get_Width(This,pWidth)

#define IAcadDocument_put_Height(This,pHeight)	\
    (This)->lpVtbl -> put_Height(This,pHeight)

#define IAcadDocument_get_Height(This,pHeight)	\
    (This)->lpVtbl -> get_Height(This,pHeight)

#define IAcadDocument_put_ActiveLayout(This,pLayout)	\
    (This)->lpVtbl -> put_ActiveLayout(This,pLayout)

#define IAcadDocument_get_ActiveLayout(This,pLayout)	\
    (This)->lpVtbl -> get_ActiveLayout(This,pLayout)

#define IAcadDocument_SendCommand(This,Command)	\
    (This)->lpVtbl -> SendCommand(This,Command)

#define IAcadDocument_get_HWND(This,HWND)	\
    (This)->lpVtbl -> get_HWND(This,HWND)

#define IAcadDocument_get_WindowTitle(This,Title)	\
    (This)->lpVtbl -> get_WindowTitle(This,Title)

#define IAcadDocument_get_Application(This,pAppObj)	\
    (This)->lpVtbl -> get_Application(This,pAppObj)

#define IAcadDocument_get_Database(This,pDatabase)	\
    (This)->lpVtbl -> get_Database(This,pDatabase)

#define IAcadDocument_StartUndoMark(This)	\
    (This)->lpVtbl -> StartUndoMark(This)

#define IAcadDocument_EndUndoMark(This)	\
    (This)->lpVtbl -> EndUndoMark(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_Plot_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pPlot);


void __RPC_STUB IAcadDocument_get_Plot_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_ActiveLayer_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ IAcadLayer __RPC_FAR *__RPC_FAR *pActLayer);


void __RPC_STUB IAcadDocument_get_ActiveLayer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_ActiveLayer_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ IAcadLayer __RPC_FAR *pActLayer);


void __RPC_STUB IAcadDocument_put_ActiveLayer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_ActiveLinetype_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ IAcadLineType __RPC_FAR *__RPC_FAR *pActLinetype);


void __RPC_STUB IAcadDocument_get_ActiveLinetype_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_ActiveLinetype_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ IAcadLineType __RPC_FAR *pActLinetype);


void __RPC_STUB IAcadDocument_put_ActiveLinetype_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_ActiveDimStyle_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ IAcadDimStyle __RPC_FAR *__RPC_FAR *pActDimStyle);


void __RPC_STUB IAcadDocument_get_ActiveDimStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_ActiveDimStyle_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ IAcadDimStyle __RPC_FAR *pActDimStyle);


void __RPC_STUB IAcadDocument_put_ActiveDimStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_ActiveTextStyle_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ IAcadTextStyle __RPC_FAR *__RPC_FAR *pActTextStyle);


void __RPC_STUB IAcadDocument_get_ActiveTextStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_ActiveTextStyle_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ IAcadTextStyle __RPC_FAR *pActTextStyle);


void __RPC_STUB IAcadDocument_put_ActiveTextStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_ActiveUCS_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ IAcadUCS __RPC_FAR *__RPC_FAR *pActUCS);


void __RPC_STUB IAcadDocument_get_ActiveUCS_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_ActiveUCS_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ IAcadUCS __RPC_FAR *pActUCS);


void __RPC_STUB IAcadDocument_put_ActiveUCS_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_ActiveViewport_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ IAcadViewport __RPC_FAR *__RPC_FAR *pActView);


void __RPC_STUB IAcadDocument_get_ActiveViewport_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_ActiveViewport_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ IAcadViewport __RPC_FAR *pActView);


void __RPC_STUB IAcadDocument_put_ActiveViewport_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_ActivePViewport_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ IAcadPViewport __RPC_FAR *__RPC_FAR *pActView);


void __RPC_STUB IAcadDocument_get_ActivePViewport_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_ActivePViewport_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ IAcadPViewport __RPC_FAR *pActView);


void __RPC_STUB IAcadDocument_put_ActivePViewport_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_ActiveSpace_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ AcActiveSpace __RPC_FAR *ActSpace);


void __RPC_STUB IAcadDocument_get_ActiveSpace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_ActiveSpace_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ AcActiveSpace ActSpace);


void __RPC_STUB IAcadDocument_put_ActiveSpace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_SelectionSets_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ IAcadSelectionSets __RPC_FAR *__RPC_FAR *pSelSets);


void __RPC_STUB IAcadDocument_get_SelectionSets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_ActiveSelectionSet_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ IAcadSelectionSet __RPC_FAR *__RPC_FAR *pSelSet);


void __RPC_STUB IAcadDocument_get_ActiveSelectionSet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_FullName_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *FullName);


void __RPC_STUB IAcadDocument_get_FullName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_Name_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Name);


void __RPC_STUB IAcadDocument_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_Path_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadDocument_get_Path_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_ObjectSnapMode_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *fSnapMode);


void __RPC_STUB IAcadDocument_get_ObjectSnapMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_ObjectSnapMode_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL fSnapMode);


void __RPC_STUB IAcadDocument_put_ObjectSnapMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_ReadOnly_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *bReadOnly);


void __RPC_STUB IAcadDocument_get_ReadOnly_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_Saved_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *bSaved);


void __RPC_STUB IAcadDocument_get_Saved_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_MSpace_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Mode);


void __RPC_STUB IAcadDocument_get_MSpace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_MSpace_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL Mode);


void __RPC_STUB IAcadDocument_put_MSpace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_Utility_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ IAcadUtility __RPC_FAR *__RPC_FAR *pUtil);


void __RPC_STUB IAcadDocument_get_Utility_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_Open_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ BSTR FullName,
    /* [optional][in] */ VARIANT Password,
    /* [retval][out] */ IAcadDocument __RPC_FAR *__RPC_FAR *pDocObj);


void __RPC_STUB IAcadDocument_Open_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_AuditInfo_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL FixErr);


void __RPC_STUB IAcadDocument_AuditInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_Import_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ BSTR FileName,
    /* [in] */ VARIANT InsertionPoint,
    /* [in] */ double ScaleFactor,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pObj);


void __RPC_STUB IAcadDocument_Import_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_Export_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ BSTR FileName,
    /* [in] */ BSTR Extension,
    /* [in] */ IAcadSelectionSet __RPC_FAR *SelectionSet);


void __RPC_STUB IAcadDocument_Export_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_New_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ BSTR TemplateFileName,
    /* [retval][out] */ IAcadDocument __RPC_FAR *__RPC_FAR *pDocObj);


void __RPC_STUB IAcadDocument_New_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_Save_Proxy( 
    IAcadDocument __RPC_FAR * This);


void __RPC_STUB IAcadDocument_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_SaveAs_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ BSTR FullFileName,
    /* [optional][in] */ VARIANT SaveAsType,
    /* [optional][in] */ VARIANT vSecurityParams);


void __RPC_STUB IAcadDocument_SaveAs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_Wblock_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ BSTR FileName,
    /* [in] */ IAcadSelectionSet __RPC_FAR *SelectionSet);


void __RPC_STUB IAcadDocument_Wblock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_PurgeAll_Proxy( 
    IAcadDocument __RPC_FAR * This);


void __RPC_STUB IAcadDocument_PurgeAll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_GetVariable_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ BSTR Name,
    /* [retval][out] */ VARIANT __RPC_FAR *Value);


void __RPC_STUB IAcadDocument_GetVariable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_SetVariable_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ BSTR Name,
    /* [in] */ VARIANT Value);


void __RPC_STUB IAcadDocument_SetVariable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_LoadShapeFile_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ BSTR FullName);


void __RPC_STUB IAcadDocument_LoadShapeFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_Regen_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ AcRegenType WhichViewports);


void __RPC_STUB IAcadDocument_Regen_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_PickfirstSelectionSet_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pSelSet);


void __RPC_STUB IAcadDocument_get_PickfirstSelectionSet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_Active_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pvbActive);


void __RPC_STUB IAcadDocument_get_Active_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_Activate_Proxy( 
    IAcadDocument __RPC_FAR * This);


void __RPC_STUB IAcadDocument_Activate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_Close_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [optional][in] */ VARIANT SaveChanges,
    /* [optional][in] */ VARIANT FileName);


void __RPC_STUB IAcadDocument_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_WindowState_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ AcWindowState pWinState);


void __RPC_STUB IAcadDocument_put_WindowState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_WindowState_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ AcWindowState __RPC_FAR *pWinState);


void __RPC_STUB IAcadDocument_get_WindowState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_Width_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ int pWidth);


void __RPC_STUB IAcadDocument_put_Width_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_Width_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *pWidth);


void __RPC_STUB IAcadDocument_get_Width_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_Height_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ int pHeight);


void __RPC_STUB IAcadDocument_put_Height_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_Height_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *pHeight);


void __RPC_STUB IAcadDocument_get_Height_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_ActiveLayout_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ IAcadLayout __RPC_FAR *pLayout);


void __RPC_STUB IAcadDocument_put_ActiveLayout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_ActiveLayout_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ IAcadLayout __RPC_FAR *__RPC_FAR *pLayout);


void __RPC_STUB IAcadDocument_get_ActiveLayout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_SendCommand_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [in] */ BSTR Command);


void __RPC_STUB IAcadDocument_SendCommand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_HWND_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *HWND);


void __RPC_STUB IAcadDocument_get_HWND_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_WindowTitle_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Title);


void __RPC_STUB IAcadDocument_get_WindowTitle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_Application_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ IAcadApplication __RPC_FAR *__RPC_FAR *pAppObj);


void __RPC_STUB IAcadDocument_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_Database_Proxy( 
    IAcadDocument __RPC_FAR * This,
    /* [retval][out] */ IAcadDatabase __RPC_FAR *__RPC_FAR *pDatabase);


void __RPC_STUB IAcadDocument_get_Database_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_StartUndoMark_Proxy( 
    IAcadDocument __RPC_FAR * This);


void __RPC_STUB IAcadDocument_StartUndoMark_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_EndUndoMark_Proxy( 
    IAcadDocument __RPC_FAR * This);


void __RPC_STUB IAcadDocument_EndUndoMark_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAcadDocument_INTERFACE_DEFINED__ */


#ifndef __IAcadDocuments_INTERFACE_DEFINED__
#define __IAcadDocuments_INTERFACE_DEFINED__

/* interface IAcadDocuments */
/* [oleautomation][dual][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IAcadDocuments;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CE3683C2-A49B-4AE7-AC30-E4968CC70ED4")
    IAcadDocuments : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IAcadDocument __RPC_FAR *__RPC_FAR *pItem) = 0;
        
        virtual /* [hidden][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pEnumVariant) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *Count) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IAcadApplication __RPC_FAR *__RPC_FAR *pAppObj) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [optional][in] */ VARIANT TemplateName,
            /* [retval][out] */ IAcadDocument __RPC_FAR *__RPC_FAR *pDispDoc) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Open( 
            /* [in] */ BSTR Name,
            /* [optional][in] */ VARIANT ReadOnly,
            /* [optional][in] */ VARIANT Password,
            /* [retval][out] */ IAcadDocument __RPC_FAR *__RPC_FAR *pDispDoc) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAcadDocumentsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAcadDocuments __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAcadDocuments __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAcadDocuments __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAcadDocuments __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAcadDocuments __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAcadDocuments __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAcadDocuments __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Item )( 
            IAcadDocuments __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IAcadDocument __RPC_FAR *__RPC_FAR *pItem);
        
        /* [hidden][restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IAcadDocuments __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pEnumVariant);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IAcadDocuments __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *Count);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Application )( 
            IAcadDocuments __RPC_FAR * This,
            /* [retval][out] */ IAcadApplication __RPC_FAR *__RPC_FAR *pAppObj);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            IAcadDocuments __RPC_FAR * This,
            /* [optional][in] */ VARIANT TemplateName,
            /* [retval][out] */ IAcadDocument __RPC_FAR *__RPC_FAR *pDispDoc);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Open )( 
            IAcadDocuments __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [optional][in] */ VARIANT ReadOnly,
            /* [optional][in] */ VARIANT Password,
            /* [retval][out] */ IAcadDocument __RPC_FAR *__RPC_FAR *pDispDoc);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Close )( 
            IAcadDocuments __RPC_FAR * This);
        
        END_INTERFACE
    } IAcadDocumentsVtbl;

    interface IAcadDocuments
    {
        CONST_VTBL struct IAcadDocumentsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAcadDocuments_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAcadDocuments_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAcadDocuments_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAcadDocuments_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAcadDocuments_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAcadDocuments_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAcadDocuments_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAcadDocuments_Item(This,Index,pItem)	\
    (This)->lpVtbl -> Item(This,Index,pItem)

#define IAcadDocuments_get__NewEnum(This,pEnumVariant)	\
    (This)->lpVtbl -> get__NewEnum(This,pEnumVariant)

#define IAcadDocuments_get_Count(This,Count)	\
    (This)->lpVtbl -> get_Count(This,Count)

#define IAcadDocuments_get_Application(This,pAppObj)	\
    (This)->lpVtbl -> get_Application(This,pAppObj)

#define IAcadDocuments_Add(This,TemplateName,pDispDoc)	\
    (This)->lpVtbl -> Add(This,TemplateName,pDispDoc)

#define IAcadDocuments_Open(This,Name,ReadOnly,Password,pDispDoc)	\
    (This)->lpVtbl -> Open(This,Name,ReadOnly,Password,pDispDoc)

#define IAcadDocuments_Close(This)	\
    (This)->lpVtbl -> Close(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocuments_Item_Proxy( 
    IAcadDocuments __RPC_FAR * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ IAcadDocument __RPC_FAR *__RPC_FAR *pItem);


void __RPC_STUB IAcadDocuments_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocuments_get__NewEnum_Proxy( 
    IAcadDocuments __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pEnumVariant);


void __RPC_STUB IAcadDocuments_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocuments_get_Count_Proxy( 
    IAcadDocuments __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *Count);


void __RPC_STUB IAcadDocuments_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocuments_get_Application_Proxy( 
    IAcadDocuments __RPC_FAR * This,
    /* [retval][out] */ IAcadApplication __RPC_FAR *__RPC_FAR *pAppObj);


void __RPC_STUB IAcadDocuments_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocuments_Add_Proxy( 
    IAcadDocuments __RPC_FAR * This,
    /* [optional][in] */ VARIANT TemplateName,
    /* [retval][out] */ IAcadDocument __RPC_FAR *__RPC_FAR *pDispDoc);


void __RPC_STUB IAcadDocuments_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocuments_Open_Proxy( 
    IAcadDocuments __RPC_FAR * This,
    /* [in] */ BSTR Name,
    /* [optional][in] */ VARIANT ReadOnly,
    /* [optional][in] */ VARIANT Password,
    /* [retval][out] */ IAcadDocument __RPC_FAR *__RPC_FAR *pDispDoc);


void __RPC_STUB IAcadDocuments_Open_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocuments_Close_Proxy( 
    IAcadDocuments __RPC_FAR * This);


void __RPC_STUB IAcadDocuments_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAcadDocuments_INTERFACE_DEFINED__ */


#ifndef __IAcadSelectionSets_INTERFACE_DEFINED__
#define __IAcadSelectionSets_INTERFACE_DEFINED__

/* interface IAcadSelectionSets */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IAcadSelectionSets;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6CD414B6-7119-426F-B8A0-000855AF8D60")
    IAcadSelectionSets : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IAcadSelectionSet __RPC_FAR *__RPC_FAR *pItem) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [hidden][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IAcadApplication __RPC_FAR *__RPC_FAR *pAppObj) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR Name,
            /* [retval][out] */ IAcadSelectionSet __RPC_FAR *__RPC_FAR *pSet) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAcadSelectionSetsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAcadSelectionSets __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAcadSelectionSets __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAcadSelectionSets __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAcadSelectionSets __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAcadSelectionSets __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAcadSelectionSets __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAcadSelectionSets __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Item )( 
            IAcadSelectionSets __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IAcadSelectionSet __RPC_FAR *__RPC_FAR *pItem);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IAcadSelectionSets __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [hidden][restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IAcadSelectionSets __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Application )( 
            IAcadSelectionSets __RPC_FAR * This,
            /* [retval][out] */ IAcadApplication __RPC_FAR *__RPC_FAR *pAppObj);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            IAcadSelectionSets __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [retval][out] */ IAcadSelectionSet __RPC_FAR *__RPC_FAR *pSet);
        
        END_INTERFACE
    } IAcadSelectionSetsVtbl;

    interface IAcadSelectionSets
    {
        CONST_VTBL struct IAcadSelectionSetsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAcadSelectionSets_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAcadSelectionSets_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAcadSelectionSets_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAcadSelectionSets_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAcadSelectionSets_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAcadSelectionSets_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAcadSelectionSets_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAcadSelectionSets_Item(This,Index,pItem)	\
    (This)->lpVtbl -> Item(This,Index,pItem)

#define IAcadSelectionSets_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IAcadSelectionSets_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IAcadSelectionSets_get_Application(This,pAppObj)	\
    (This)->lpVtbl -> get_Application(This,pAppObj)

#define IAcadSelectionSets_Add(This,Name,pSet)	\
    (This)->lpVtbl -> Add(This,Name,pSet)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSets_Item_Proxy( 
    IAcadSelectionSets __RPC_FAR * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ IAcadSelectionSet __RPC_FAR *__RPC_FAR *pItem);


void __RPC_STUB IAcadSelectionSets_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSets_get_Count_Proxy( 
    IAcadSelectionSets __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAcadSelectionSets_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSets_get__NewEnum_Proxy( 
    IAcadSelectionSets __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IAcadSelectionSets_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSets_get_Application_Proxy( 
    IAcadSelectionSets __RPC_FAR * This,
    /* [retval][out] */ IAcadApplication __RPC_FAR *__RPC_FAR *pAppObj);


void __RPC_STUB IAcadSelectionSets_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSets_Add_Proxy( 
    IAcadSelectionSets __RPC_FAR * This,
    /* [in] */ BSTR Name,
    /* [retval][out] */ IAcadSelectionSet __RPC_FAR *__RPC_FAR *pSet);


void __RPC_STUB IAcadSelectionSets_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAcadSelectionSets_INTERFACE_DEFINED__ */


#ifndef __IAcadSelectionSet_INTERFACE_DEFINED__
#define __IAcadSelectionSet_INTERFACE_DEFINED__

/* interface IAcadSelectionSet */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IAcadSelectionSet;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6729D2C8-E1AF-4248-914D-F208A0E05E84")
    IAcadSelectionSet : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IAcadEntity __RPC_FAR *__RPC_FAR *pEntity) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [hidden][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *bstrName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Highlight( 
            /* [in] */ VARIANT_BOOL bFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Erase( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Update( void) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IAcadApplication __RPC_FAR *__RPC_FAR *pAppObj) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddItems( 
            /* [in] */ VARIANT pSelSet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemoveItems( 
            /* [in] */ VARIANT Objects) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Select( 
            /* [in] */ AcSelect Mode,
            /* [optional][in] */ VARIANT Point1,
            /* [optional][in] */ VARIANT Point2,
            /* [optional][in] */ VARIANT FilterType,
            /* [optional][in] */ VARIANT FilterData) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SelectAtPoint( 
            /* [in] */ VARIANT Point,
            /* [optional][in] */ VARIANT FilterType,
            /* [optional][in] */ VARIANT FilterData) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SelectByPolygon( 
            /* [in] */ AcSelect Mode,
            /* [in] */ VARIANT PointsList,
            /* [optional][in] */ VARIANT FilterType,
            /* [optional][in] */ VARIANT FilterData) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SelectOnScreen( 
            /* [optional][in] */ VARIANT FilterType,
            /* [optional][in] */ VARIANT FilterData) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Delete( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAcadSelectionSetVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAcadSelectionSet __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAcadSelectionSet __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAcadSelectionSet __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAcadSelectionSet __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAcadSelectionSet __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAcadSelectionSet __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAcadSelectionSet __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Item )( 
            IAcadSelectionSet __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IAcadEntity __RPC_FAR *__RPC_FAR *pEntity);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IAcadSelectionSet __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [hidden][restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IAcadSelectionSet __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IAcadSelectionSet __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *bstrName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Highlight )( 
            IAcadSelectionSet __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Erase )( 
            IAcadSelectionSet __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Update )( 
            IAcadSelectionSet __RPC_FAR * This);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Application )( 
            IAcadSelectionSet __RPC_FAR * This,
            /* [retval][out] */ IAcadApplication __RPC_FAR *__RPC_FAR *pAppObj);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddItems )( 
            IAcadSelectionSet __RPC_FAR * This,
            /* [in] */ VARIANT pSelSet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RemoveItems )( 
            IAcadSelectionSet __RPC_FAR * This,
            /* [in] */ VARIANT Objects);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clear )( 
            IAcadSelectionSet __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Select )( 
            IAcadSelectionSet __RPC_FAR * This,
            /* [in] */ AcSelect Mode,
            /* [optional][in] */ VARIANT Point1,
            /* [optional][in] */ VARIANT Point2,
            /* [optional][in] */ VARIANT FilterType,
            /* [optional][in] */ VARIANT FilterData);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SelectAtPoint )( 
            IAcadSelectionSet __RPC_FAR * This,
            /* [in] */ VARIANT Point,
            /* [optional][in] */ VARIANT FilterType,
            /* [optional][in] */ VARIANT FilterData);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SelectByPolygon )( 
            IAcadSelectionSet __RPC_FAR * This,
            /* [in] */ AcSelect Mode,
            /* [in] */ VARIANT PointsList,
            /* [optional][in] */ VARIANT FilterType,
            /* [optional][in] */ VARIANT FilterData);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SelectOnScreen )( 
            IAcadSelectionSet __RPC_FAR * This,
            /* [optional][in] */ VARIANT FilterType,
            /* [optional][in] */ VARIANT FilterData);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            IAcadSelectionSet __RPC_FAR * This);
        
        END_INTERFACE
    } IAcadSelectionSetVtbl;

    interface IAcadSelectionSet
    {
        CONST_VTBL struct IAcadSelectionSetVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAcadSelectionSet_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAcadSelectionSet_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAcadSelectionSet_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAcadSelectionSet_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAcadSelectionSet_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAcadSelectionSet_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAcadSelectionSet_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAcadSelectionSet_Item(This,Index,pEntity)	\
    (This)->lpVtbl -> Item(This,Index,pEntity)

#define IAcadSelectionSet_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IAcadSelectionSet_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IAcadSelectionSet_get_Name(This,bstrName)	\
    (This)->lpVtbl -> get_Name(This,bstrName)

#define IAcadSelectionSet_Highlight(This,bFlag)	\
    (This)->lpVtbl -> Highlight(This,bFlag)

#define IAcadSelectionSet_Erase(This)	\
    (This)->lpVtbl -> Erase(This)

#define IAcadSelectionSet_Update(This)	\
    (This)->lpVtbl -> Update(This)

#define IAcadSelectionSet_get_Application(This,pAppObj)	\
    (This)->lpVtbl -> get_Application(This,pAppObj)

#define IAcadSelectionSet_AddItems(This,pSelSet)	\
    (This)->lpVtbl -> AddItems(This,pSelSet)

#define IAcadSelectionSet_RemoveItems(This,Objects)	\
    (This)->lpVtbl -> RemoveItems(This,Objects)

#define IAcadSelectionSet_Clear(This)	\
    (This)->lpVtbl -> Clear(This)

#define IAcadSelectionSet_Select(This,Mode,Point1,Point2,FilterType,FilterData)	\
    (This)->lpVtbl -> Select(This,Mode,Point1,Point2,FilterType,FilterData)

#define IAcadSelectionSet_SelectAtPoint(This,Point,FilterType,FilterData)	\
    (This)->lpVtbl -> SelectAtPoint(This,Point,FilterType,FilterData)

#define IAcadSelectionSet_SelectByPolygon(This,Mode,PointsList,FilterType,FilterData)	\
    (This)->lpVtbl -> SelectByPolygon(This,Mode,PointsList,FilterType,FilterData)

#define IAcadSelectionSet_SelectOnScreen(This,FilterType,FilterData)	\
    (This)->lpVtbl -> SelectOnScreen(This,FilterType,FilterData)

#define IAcadSelectionSet_Delete(This)	\
    (This)->lpVtbl -> Delete(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_Item_Proxy( 
    IAcadSelectionSet __RPC_FAR * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ IAcadEntity __RPC_FAR *__RPC_FAR *pEntity);


void __RPC_STUB IAcadSelectionSet_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_get_Count_Proxy( 
    IAcadSelectionSet __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAcadSelectionSet_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_get__NewEnum_Proxy( 
    IAcadSelectionSet __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IAcadSelectionSet_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_get_Name_Proxy( 
    IAcadSelectionSet __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *bstrName);


void __RPC_STUB IAcadSelectionSet_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_Highlight_Proxy( 
    IAcadSelectionSet __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bFlag);


void __RPC_STUB IAcadSelectionSet_Highlight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_Erase_Proxy( 
    IAcadSelectionSet __RPC_FAR * This);


void __RPC_STUB IAcadSelectionSet_Erase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_Update_Proxy( 
    IAcadSelectionSet __RPC_FAR * This);


void __RPC_STUB IAcadSelectionSet_Update_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_get_Application_Proxy( 
    IAcadSelectionSet __RPC_FAR * This,
    /* [retval][out] */ IAcadApplication __RPC_FAR *__RPC_FAR *pAppObj);


void __RPC_STUB IAcadSelectionSet_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_AddItems_Proxy( 
    IAcadSelectionSet __RPC_FAR * This,
    /* [in] */ VARIANT pSelSet);


void __RPC_STUB IAcadSelectionSet_AddItems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_RemoveItems_Proxy( 
    IAcadSelectionSet __RPC_FAR * This,
    /* [in] */ VARIANT Objects);


void __RPC_STUB IAcadSelectionSet_RemoveItems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_Clear_Proxy( 
    IAcadSelectionSet __RPC_FAR * This);


void __RPC_STUB IAcadSelectionSet_Clear_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_Select_Proxy( 
    IAcadSelectionSet __RPC_FAR * This,
    /* [in] */ AcSelect Mode,
    /* [optional][in] */ VARIANT Point1,
    /* [optional][in] */ VARIANT Point2,
    /* [optional][in] */ VARIANT FilterType,
    /* [optional][in] */ VARIANT FilterData);


void __RPC_STUB IAcadSelectionSet_Select_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_SelectAtPoint_Proxy( 
    IAcadSelectionSet __RPC_FAR * This,
    /* [in] */ VARIANT Point,
    /* [optional][in] */ VARIANT FilterType,
    /* [optional][in] */ VARIANT FilterData);


void __RPC_STUB IAcadSelectionSet_SelectAtPoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_SelectByPolygon_Proxy( 
    IAcadSelectionSet __RPC_FAR * This,
    /* [in] */ AcSelect Mode,
    /* [in] */ VARIANT PointsList,
    /* [optional][in] */ VARIANT FilterType,
    /* [optional][in] */ VARIANT FilterData);


void __RPC_STUB IAcadSelectionSet_SelectByPolygon_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_SelectOnScreen_Proxy( 
    IAcadSelectionSet __RPC_FAR * This,
    /* [optional][in] */ VARIANT FilterType,
    /* [optional][in] */ VARIANT FilterData);


void __RPC_STUB IAcadSelectionSet_SelectOnScreen_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_Delete_Proxy( 
    IAcadSelectionSet __RPC_FAR * This);


void __RPC_STUB IAcadSelectionSet_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAcadSelectionSet_INTERFACE_DEFINED__ */


#ifndef __IAcadPreferences_INTERFACE_DEFINED__
#define __IAcadPreferences_INTERFACE_DEFINED__

/* interface IAcadPreferences */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IAcadPreferences;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("498902DC-F5F2-4D4C-948C-D19B645F0C7F")
    IAcadPreferences : public IDispatch
    {
    public:
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IAcadApplication __RPC_FAR *__RPC_FAR *pAppObj) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Files( 
            /* [retval][out] */ IAcadPreferencesFiles __RPC_FAR *__RPC_FAR *pObj) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAcadPreferencesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAcadPreferences __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAcadPreferences __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAcadPreferences __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAcadPreferences __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAcadPreferences __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAcadPreferences __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAcadPreferences __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Application )( 
            IAcadPreferences __RPC_FAR * This,
            /* [retval][out] */ IAcadApplication __RPC_FAR *__RPC_FAR *pAppObj);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Files )( 
            IAcadPreferences __RPC_FAR * This,
            /* [retval][out] */ IAcadPreferencesFiles __RPC_FAR *__RPC_FAR *pObj);
        
        END_INTERFACE
    } IAcadPreferencesVtbl;

    interface IAcadPreferences
    {
        CONST_VTBL struct IAcadPreferencesVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAcadPreferences_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAcadPreferences_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAcadPreferences_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAcadPreferences_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAcadPreferences_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAcadPreferences_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAcadPreferences_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAcadPreferences_get_Application(This,pAppObj)	\
    (This)->lpVtbl -> get_Application(This,pAppObj)

#define IAcadPreferences_get_Files(This,pObj)	\
    (This)->lpVtbl -> get_Files(This,pObj)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferences_get_Application_Proxy( 
    IAcadPreferences __RPC_FAR * This,
    /* [retval][out] */ IAcadApplication __RPC_FAR *__RPC_FAR *pAppObj);


void __RPC_STUB IAcadPreferences_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferences_get_Files_Proxy( 
    IAcadPreferences __RPC_FAR * This,
    /* [retval][out] */ IAcadPreferencesFiles __RPC_FAR *__RPC_FAR *pObj);


void __RPC_STUB IAcadPreferences_get_Files_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAcadPreferences_INTERFACE_DEFINED__ */


#ifndef __IAcadPreferencesFiles_INTERFACE_DEFINED__
#define __IAcadPreferencesFiles_INTERFACE_DEFINED__

/* interface IAcadPreferencesFiles */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IAcadPreferencesFiles;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2443685E-ED1D-4753-AA91-963CB86AE095")
    IAcadPreferencesFiles : public IDispatch
    {
    public:
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IAcadApplication __RPC_FAR *__RPC_FAR *pAppObj) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_SupportPath( 
            /* [in] */ BSTR orient) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_SupportPath( 
            /* [retval][out] */ BSTR __RPC_FAR *orient) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_DriversPath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_DriversPath( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_MenuFile( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_MenuFile( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_HelpFilePath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_HelpFilePath( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_DefaultInternetURL( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_DefaultInternetURL( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ConfigFile( 
            /* [retval][out] */ BSTR __RPC_FAR *ConfigFile) = 0;
        
        virtual /* [helpstring][hidden][propget][id] */ HRESULT STDMETHODCALLTYPE get_LicenseServer( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_TextEditor( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_TextEditor( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_MainDictionary( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_MainDictionary( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_CustomDictionary( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_CustomDictionary( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_AltFontFile( 
            /* [in] */ BSTR fontFile) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_AltFontFile( 
            /* [retval][out] */ BSTR __RPC_FAR *fontFile) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_FontFileMap( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_FontFileMap( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_PrintFile( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_PrintFile( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_PrintSpoolExecutable( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_PrintSpoolExecutable( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_PostScriptPrologFile( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_PostScriptPrologFile( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_PrintSpoolerPath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_PrintSpoolerPath( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_AutoSavePath( 
            /* [in] */ BSTR AutoSavePath) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_AutoSavePath( 
            /* [retval][out] */ BSTR __RPC_FAR *AutoSavePath) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_TemplateDwgPath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_TemplateDwgPath( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_LogFilePath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_LogFilePath( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_TempFilePath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_TempFilePath( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_TempXrefPath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_TempXrefPath( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_TextureMapPath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_TextureMapPath( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_AltTabletMenuFile( 
            /* [in] */ BSTR MenuFile) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_AltTabletMenuFile( 
            /* [retval][out] */ BSTR __RPC_FAR *MenuFile) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetProjectFilePath( 
            /* [in] */ BSTR ProjectName,
            /* [in] */ BSTR ProjectFilePath) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetProjectFilePath( 
            /* [in] */ BSTR ProjectName,
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_PrinterConfigPath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_PrinterConfigPath( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_PrinterDescPath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_PrinterDescPath( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_PrinterStyleSheetPath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_PrinterStyleSheetPath( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_WorkspacePath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_WorkspacePath( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][hidden][propput][id] */ HRESULT STDMETHODCALLTYPE put_ObjectARXPath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][hidden][propget][id] */ HRESULT STDMETHODCALLTYPE get_ObjectARXPath( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ColorBookPath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ColorBookPath( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ToolPalettePath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ToolPalettePath( 
            /* [retval][out] */ BSTR __RPC_FAR *Path) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAcadPreferencesFilesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAcadPreferencesFiles __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAcadPreferencesFiles __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Application )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ IAcadApplication __RPC_FAR *__RPC_FAR *pAppObj);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SupportPath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR orient);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SupportPath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *orient);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DriversPath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DriversPath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MenuFile )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MenuFile )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HelpFilePath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HelpFilePath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DefaultInternetURL )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DefaultInternetURL )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ConfigFile )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *ConfigFile);
        
        /* [helpstring][hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LicenseServer )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TextEditor )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TextEditor )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MainDictionary )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MainDictionary )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CustomDictionary )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CustomDictionary )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AltFontFile )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR fontFile);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AltFontFile )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *fontFile);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FontFileMap )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FontFileMap )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PrintFile )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PrintFile )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PrintSpoolExecutable )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PrintSpoolExecutable )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PostScriptPrologFile )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PostScriptPrologFile )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PrintSpoolerPath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PrintSpoolerPath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AutoSavePath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR AutoSavePath);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AutoSavePath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *AutoSavePath);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TemplateDwgPath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TemplateDwgPath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LogFilePath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LogFilePath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TempFilePath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TempFilePath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TempXrefPath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TempXrefPath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TextureMapPath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TextureMapPath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AltTabletMenuFile )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR MenuFile);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AltTabletMenuFile )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *MenuFile);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetProjectFilePath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR ProjectName,
            /* [in] */ BSTR ProjectFilePath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetProjectFilePath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR ProjectName,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PrinterConfigPath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PrinterConfigPath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PrinterDescPath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PrinterDescPath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PrinterStyleSheetPath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PrinterStyleSheetPath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WorkspacePath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WorkspacePath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][hidden][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ObjectARXPath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ObjectARXPath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ColorBookPath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ColorBookPath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ToolPalettePath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ToolPalettePath )( 
            IAcadPreferencesFiles __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Path);
        
        END_INTERFACE
    } IAcadPreferencesFilesVtbl;

    interface IAcadPreferencesFiles
    {
        CONST_VTBL struct IAcadPreferencesFilesVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAcadPreferencesFiles_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAcadPreferencesFiles_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAcadPreferencesFiles_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAcadPreferencesFiles_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAcadPreferencesFiles_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAcadPreferencesFiles_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAcadPreferencesFiles_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAcadPreferencesFiles_get_Application(This,pAppObj)	\
    (This)->lpVtbl -> get_Application(This,pAppObj)

#define IAcadPreferencesFiles_put_SupportPath(This,orient)	\
    (This)->lpVtbl -> put_SupportPath(This,orient)

#define IAcadPreferencesFiles_get_SupportPath(This,orient)	\
    (This)->lpVtbl -> get_SupportPath(This,orient)

#define IAcadPreferencesFiles_put_DriversPath(This,Path)	\
    (This)->lpVtbl -> put_DriversPath(This,Path)

#define IAcadPreferencesFiles_get_DriversPath(This,Path)	\
    (This)->lpVtbl -> get_DriversPath(This,Path)

#define IAcadPreferencesFiles_put_MenuFile(This,Path)	\
    (This)->lpVtbl -> put_MenuFile(This,Path)

#define IAcadPreferencesFiles_get_MenuFile(This,Path)	\
    (This)->lpVtbl -> get_MenuFile(This,Path)

#define IAcadPreferencesFiles_put_HelpFilePath(This,Path)	\
    (This)->lpVtbl -> put_HelpFilePath(This,Path)

#define IAcadPreferencesFiles_get_HelpFilePath(This,Path)	\
    (This)->lpVtbl -> get_HelpFilePath(This,Path)

#define IAcadPreferencesFiles_put_DefaultInternetURL(This,Path)	\
    (This)->lpVtbl -> put_DefaultInternetURL(This,Path)

#define IAcadPreferencesFiles_get_DefaultInternetURL(This,Path)	\
    (This)->lpVtbl -> get_DefaultInternetURL(This,Path)

#define IAcadPreferencesFiles_get_ConfigFile(This,ConfigFile)	\
    (This)->lpVtbl -> get_ConfigFile(This,ConfigFile)

#define IAcadPreferencesFiles_get_LicenseServer(This,Path)	\
    (This)->lpVtbl -> get_LicenseServer(This,Path)

#define IAcadPreferencesFiles_put_TextEditor(This,Path)	\
    (This)->lpVtbl -> put_TextEditor(This,Path)

#define IAcadPreferencesFiles_get_TextEditor(This,Path)	\
    (This)->lpVtbl -> get_TextEditor(This,Path)

#define IAcadPreferencesFiles_put_MainDictionary(This,Path)	\
    (This)->lpVtbl -> put_MainDictionary(This,Path)

#define IAcadPreferencesFiles_get_MainDictionary(This,Path)	\
    (This)->lpVtbl -> get_MainDictionary(This,Path)

#define IAcadPreferencesFiles_put_CustomDictionary(This,Path)	\
    (This)->lpVtbl -> put_CustomDictionary(This,Path)

#define IAcadPreferencesFiles_get_CustomDictionary(This,Path)	\
    (This)->lpVtbl -> get_CustomDictionary(This,Path)

#define IAcadPreferencesFiles_put_AltFontFile(This,fontFile)	\
    (This)->lpVtbl -> put_AltFontFile(This,fontFile)

#define IAcadPreferencesFiles_get_AltFontFile(This,fontFile)	\
    (This)->lpVtbl -> get_AltFontFile(This,fontFile)

#define IAcadPreferencesFiles_put_FontFileMap(This,Path)	\
    (This)->lpVtbl -> put_FontFileMap(This,Path)

#define IAcadPreferencesFiles_get_FontFileMap(This,Path)	\
    (This)->lpVtbl -> get_FontFileMap(This,Path)

#define IAcadPreferencesFiles_put_PrintFile(This,Path)	\
    (This)->lpVtbl -> put_PrintFile(This,Path)

#define IAcadPreferencesFiles_get_PrintFile(This,Path)	\
    (This)->lpVtbl -> get_PrintFile(This,Path)

#define IAcadPreferencesFiles_put_PrintSpoolExecutable(This,Path)	\
    (This)->lpVtbl -> put_PrintSpoolExecutable(This,Path)

#define IAcadPreferencesFiles_get_PrintSpoolExecutable(This,Path)	\
    (This)->lpVtbl -> get_PrintSpoolExecutable(This,Path)

#define IAcadPreferencesFiles_put_PostScriptPrologFile(This,Path)	\
    (This)->lpVtbl -> put_PostScriptPrologFile(This,Path)

#define IAcadPreferencesFiles_get_PostScriptPrologFile(This,Path)	\
    (This)->lpVtbl -> get_PostScriptPrologFile(This,Path)

#define IAcadPreferencesFiles_put_PrintSpoolerPath(This,Path)	\
    (This)->lpVtbl -> put_PrintSpoolerPath(This,Path)

#define IAcadPreferencesFiles_get_PrintSpoolerPath(This,Path)	\
    (This)->lpVtbl -> get_PrintSpoolerPath(This,Path)

#define IAcadPreferencesFiles_put_AutoSavePath(This,AutoSavePath)	\
    (This)->lpVtbl -> put_AutoSavePath(This,AutoSavePath)

#define IAcadPreferencesFiles_get_AutoSavePath(This,AutoSavePath)	\
    (This)->lpVtbl -> get_AutoSavePath(This,AutoSavePath)

#define IAcadPreferencesFiles_put_TemplateDwgPath(This,Path)	\
    (This)->lpVtbl -> put_TemplateDwgPath(This,Path)

#define IAcadPreferencesFiles_get_TemplateDwgPath(This,Path)	\
    (This)->lpVtbl -> get_TemplateDwgPath(This,Path)

#define IAcadPreferencesFiles_put_LogFilePath(This,Path)	\
    (This)->lpVtbl -> put_LogFilePath(This,Path)

#define IAcadPreferencesFiles_get_LogFilePath(This,Path)	\
    (This)->lpVtbl -> get_LogFilePath(This,Path)

#define IAcadPreferencesFiles_put_TempFilePath(This,Path)	\
    (This)->lpVtbl -> put_TempFilePath(This,Path)

#define IAcadPreferencesFiles_get_TempFilePath(This,Path)	\
    (This)->lpVtbl -> get_TempFilePath(This,Path)

#define IAcadPreferencesFiles_put_TempXrefPath(This,Path)	\
    (This)->lpVtbl -> put_TempXrefPath(This,Path)

#define IAcadPreferencesFiles_get_TempXrefPath(This,Path)	\
    (This)->lpVtbl -> get_TempXrefPath(This,Path)

#define IAcadPreferencesFiles_put_TextureMapPath(This,Path)	\
    (This)->lpVtbl -> put_TextureMapPath(This,Path)

#define IAcadPreferencesFiles_get_TextureMapPath(This,Path)	\
    (This)->lpVtbl -> get_TextureMapPath(This,Path)

#define IAcadPreferencesFiles_put_AltTabletMenuFile(This,MenuFile)	\
    (This)->lpVtbl -> put_AltTabletMenuFile(This,MenuFile)

#define IAcadPreferencesFiles_get_AltTabletMenuFile(This,MenuFile)	\
    (This)->lpVtbl -> get_AltTabletMenuFile(This,MenuFile)

#define IAcadPreferencesFiles_SetProjectFilePath(This,ProjectName,ProjectFilePath)	\
    (This)->lpVtbl -> SetProjectFilePath(This,ProjectName,ProjectFilePath)

#define IAcadPreferencesFiles_GetProjectFilePath(This,ProjectName,Path)	\
    (This)->lpVtbl -> GetProjectFilePath(This,ProjectName,Path)

#define IAcadPreferencesFiles_put_PrinterConfigPath(This,Path)	\
    (This)->lpVtbl -> put_PrinterConfigPath(This,Path)

#define IAcadPreferencesFiles_get_PrinterConfigPath(This,Path)	\
    (This)->lpVtbl -> get_PrinterConfigPath(This,Path)

#define IAcadPreferencesFiles_put_PrinterDescPath(This,Path)	\
    (This)->lpVtbl -> put_PrinterDescPath(This,Path)

#define IAcadPreferencesFiles_get_PrinterDescPath(This,Path)	\
    (This)->lpVtbl -> get_PrinterDescPath(This,Path)

#define IAcadPreferencesFiles_put_PrinterStyleSheetPath(This,Path)	\
    (This)->lpVtbl -> put_PrinterStyleSheetPath(This,Path)

#define IAcadPreferencesFiles_get_PrinterStyleSheetPath(This,Path)	\
    (This)->lpVtbl -> get_PrinterStyleSheetPath(This,Path)

#define IAcadPreferencesFiles_put_WorkspacePath(This,Path)	\
    (This)->lpVtbl -> put_WorkspacePath(This,Path)

#define IAcadPreferencesFiles_get_WorkspacePath(This,Path)	\
    (This)->lpVtbl -> get_WorkspacePath(This,Path)

#define IAcadPreferencesFiles_put_ObjectARXPath(This,Path)	\
    (This)->lpVtbl -> put_ObjectARXPath(This,Path)

#define IAcadPreferencesFiles_get_ObjectARXPath(This,Path)	\
    (This)->lpVtbl -> get_ObjectARXPath(This,Path)

#define IAcadPreferencesFiles_put_ColorBookPath(This,Path)	\
    (This)->lpVtbl -> put_ColorBookPath(This,Path)

#define IAcadPreferencesFiles_get_ColorBookPath(This,Path)	\
    (This)->lpVtbl -> get_ColorBookPath(This,Path)

#define IAcadPreferencesFiles_put_ToolPalettePath(This,Path)	\
    (This)->lpVtbl -> put_ToolPalettePath(This,Path)

#define IAcadPreferencesFiles_get_ToolPalettePath(This,Path)	\
    (This)->lpVtbl -> get_ToolPalettePath(This,Path)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_Application_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ IAcadApplication __RPC_FAR *__RPC_FAR *pAppObj);


void __RPC_STUB IAcadPreferencesFiles_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_SupportPath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR orient);


void __RPC_STUB IAcadPreferencesFiles_put_SupportPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_SupportPath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *orient);


void __RPC_STUB IAcadPreferencesFiles_get_SupportPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_DriversPath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_DriversPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_DriversPath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_DriversPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_MenuFile_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_MenuFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_MenuFile_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_MenuFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_HelpFilePath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_HelpFilePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_HelpFilePath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_HelpFilePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_DefaultInternetURL_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_DefaultInternetURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_DefaultInternetURL_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_DefaultInternetURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_ConfigFile_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *ConfigFile);


void __RPC_STUB IAcadPreferencesFiles_get_ConfigFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][hidden][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_LicenseServer_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_LicenseServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_TextEditor_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_TextEditor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_TextEditor_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_TextEditor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_MainDictionary_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_MainDictionary_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_MainDictionary_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_MainDictionary_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_CustomDictionary_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_CustomDictionary_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_CustomDictionary_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_CustomDictionary_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_AltFontFile_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR fontFile);


void __RPC_STUB IAcadPreferencesFiles_put_AltFontFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_AltFontFile_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *fontFile);


void __RPC_STUB IAcadPreferencesFiles_get_AltFontFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_FontFileMap_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_FontFileMap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_FontFileMap_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_FontFileMap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_PrintFile_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_PrintFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_PrintFile_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_PrintFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_PrintSpoolExecutable_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_PrintSpoolExecutable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_PrintSpoolExecutable_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_PrintSpoolExecutable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_PostScriptPrologFile_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_PostScriptPrologFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_PostScriptPrologFile_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_PostScriptPrologFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_PrintSpoolerPath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_PrintSpoolerPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_PrintSpoolerPath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_PrintSpoolerPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_AutoSavePath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR AutoSavePath);


void __RPC_STUB IAcadPreferencesFiles_put_AutoSavePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_AutoSavePath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *AutoSavePath);


void __RPC_STUB IAcadPreferencesFiles_get_AutoSavePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_TemplateDwgPath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_TemplateDwgPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_TemplateDwgPath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_TemplateDwgPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_LogFilePath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_LogFilePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_LogFilePath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_LogFilePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_TempFilePath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_TempFilePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_TempFilePath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_TempFilePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_TempXrefPath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_TempXrefPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_TempXrefPath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_TempXrefPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_TextureMapPath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_TextureMapPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_TextureMapPath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_TextureMapPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_AltTabletMenuFile_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR MenuFile);


void __RPC_STUB IAcadPreferencesFiles_put_AltTabletMenuFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_AltTabletMenuFile_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *MenuFile);


void __RPC_STUB IAcadPreferencesFiles_get_AltTabletMenuFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_SetProjectFilePath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR ProjectName,
    /* [in] */ BSTR ProjectFilePath);


void __RPC_STUB IAcadPreferencesFiles_SetProjectFilePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_GetProjectFilePath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR ProjectName,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_GetProjectFilePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_PrinterConfigPath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_PrinterConfigPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_PrinterConfigPath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_PrinterConfigPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_PrinterDescPath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_PrinterDescPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_PrinterDescPath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_PrinterDescPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_PrinterStyleSheetPath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_PrinterStyleSheetPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_PrinterStyleSheetPath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_PrinterStyleSheetPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_WorkspacePath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_WorkspacePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_WorkspacePath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_WorkspacePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][hidden][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_ObjectARXPath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_ObjectARXPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][hidden][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_ObjectARXPath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_ObjectARXPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_ColorBookPath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_ColorBookPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_ColorBookPath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_ColorBookPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_ToolPalettePath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_ToolPalettePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_ToolPalettePath_Proxy( 
    IAcadPreferencesFiles __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_ToolPalettePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAcadPreferencesFiles_INTERFACE_DEFINED__ */


#ifndef __IAcadLayerStateManager_INTERFACE_DEFINED__
#define __IAcadLayerStateManager_INTERFACE_DEFINED__

/* interface IAcadLayerStateManager */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IAcadLayerStateManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("92999267-E45C-43BA-BAB0-7C408BAFADC2")
    IAcadLayerStateManager : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetDatabase( 
            /* [in] */ IAcadDatabase __RPC_FAR *iHostDb) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Mask( 
            /* [in] */ BSTR bsName,
            /* [in] */ AcLayerStateMask eMask) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Mask( 
            /* [in] */ BSTR bsName,
            /* [retval][out] */ AcLayerStateMask __RPC_FAR *eMask) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [in] */ BSTR bsName,
            /* [in] */ AcLayerStateMask eMask) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Restore( 
            /* [in] */ BSTR bsName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ BSTR bsName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Rename( 
            /* [in] */ BSTR bsName,
            /* [in] */ BSTR bsNewName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Import( 
            /* [in] */ BSTR bsFilename) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Export( 
            /* [in] */ BSTR bsName,
            /* [in] */ BSTR bsFilename) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAcadLayerStateManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAcadLayerStateManager __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAcadLayerStateManager __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAcadLayerStateManager __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAcadLayerStateManager __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAcadLayerStateManager __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAcadLayerStateManager __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAcadLayerStateManager __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDatabase )( 
            IAcadLayerStateManager __RPC_FAR * This,
            /* [in] */ IAcadDatabase __RPC_FAR *iHostDb);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Mask )( 
            IAcadLayerStateManager __RPC_FAR * This,
            /* [in] */ BSTR bsName,
            /* [in] */ AcLayerStateMask eMask);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Mask )( 
            IAcadLayerStateManager __RPC_FAR * This,
            /* [in] */ BSTR bsName,
            /* [retval][out] */ AcLayerStateMask __RPC_FAR *eMask);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            IAcadLayerStateManager __RPC_FAR * This,
            /* [in] */ BSTR bsName,
            /* [in] */ AcLayerStateMask eMask);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Restore )( 
            IAcadLayerStateManager __RPC_FAR * This,
            /* [in] */ BSTR bsName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            IAcadLayerStateManager __RPC_FAR * This,
            /* [in] */ BSTR bsName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Rename )( 
            IAcadLayerStateManager __RPC_FAR * This,
            /* [in] */ BSTR bsName,
            /* [in] */ BSTR bsNewName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Import )( 
            IAcadLayerStateManager __RPC_FAR * This,
            /* [in] */ BSTR bsFilename);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Export )( 
            IAcadLayerStateManager __RPC_FAR * This,
            /* [in] */ BSTR bsName,
            /* [in] */ BSTR bsFilename);
        
        END_INTERFACE
    } IAcadLayerStateManagerVtbl;

    interface IAcadLayerStateManager
    {
        CONST_VTBL struct IAcadLayerStateManagerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAcadLayerStateManager_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAcadLayerStateManager_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAcadLayerStateManager_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAcadLayerStateManager_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAcadLayerStateManager_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAcadLayerStateManager_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAcadLayerStateManager_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAcadLayerStateManager_SetDatabase(This,iHostDb)	\
    (This)->lpVtbl -> SetDatabase(This,iHostDb)

#define IAcadLayerStateManager_put_Mask(This,bsName,eMask)	\
    (This)->lpVtbl -> put_Mask(This,bsName,eMask)

#define IAcadLayerStateManager_get_Mask(This,bsName,eMask)	\
    (This)->lpVtbl -> get_Mask(This,bsName,eMask)

#define IAcadLayerStateManager_Save(This,bsName,eMask)	\
    (This)->lpVtbl -> Save(This,bsName,eMask)

#define IAcadLayerStateManager_Restore(This,bsName)	\
    (This)->lpVtbl -> Restore(This,bsName)

#define IAcadLayerStateManager_Delete(This,bsName)	\
    (This)->lpVtbl -> Delete(This,bsName)

#define IAcadLayerStateManager_Rename(This,bsName,bsNewName)	\
    (This)->lpVtbl -> Rename(This,bsName,bsNewName)

#define IAcadLayerStateManager_Import(This,bsFilename)	\
    (This)->lpVtbl -> Import(This,bsFilename)

#define IAcadLayerStateManager_Export(This,bsName,bsFilename)	\
    (This)->lpVtbl -> Export(This,bsName,bsFilename)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadLayerStateManager_SetDatabase_Proxy( 
    IAcadLayerStateManager __RPC_FAR * This,
    /* [in] */ IAcadDatabase __RPC_FAR *iHostDb);


void __RPC_STUB IAcadLayerStateManager_SetDatabase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadLayerStateManager_put_Mask_Proxy( 
    IAcadLayerStateManager __RPC_FAR * This,
    /* [in] */ BSTR bsName,
    /* [in] */ AcLayerStateMask eMask);


void __RPC_STUB IAcadLayerStateManager_put_Mask_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadLayerStateManager_get_Mask_Proxy( 
    IAcadLayerStateManager __RPC_FAR * This,
    /* [in] */ BSTR bsName,
    /* [retval][out] */ AcLayerStateMask __RPC_FAR *eMask);


void __RPC_STUB IAcadLayerStateManager_get_Mask_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadLayerStateManager_Save_Proxy( 
    IAcadLayerStateManager __RPC_FAR * This,
    /* [in] */ BSTR bsName,
    /* [in] */ AcLayerStateMask eMask);


void __RPC_STUB IAcadLayerStateManager_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadLayerStateManager_Restore_Proxy( 
    IAcadLayerStateManager __RPC_FAR * This,
    /* [in] */ BSTR bsName);


void __RPC_STUB IAcadLayerStateManager_Restore_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadLayerStateManager_Delete_Proxy( 
    IAcadLayerStateManager __RPC_FAR * This,
    /* [in] */ BSTR bsName);


void __RPC_STUB IAcadLayerStateManager_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadLayerStateManager_Rename_Proxy( 
    IAcadLayerStateManager __RPC_FAR * This,
    /* [in] */ BSTR bsName,
    /* [in] */ BSTR bsNewName);


void __RPC_STUB IAcadLayerStateManager_Rename_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadLayerStateManager_Import_Proxy( 
    IAcadLayerStateManager __RPC_FAR * This,
    /* [in] */ BSTR bsFilename);


void __RPC_STUB IAcadLayerStateManager_Import_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadLayerStateManager_Export_Proxy( 
    IAcadLayerStateManager __RPC_FAR * This,
    /* [in] */ BSTR bsName,
    /* [in] */ BSTR bsFilename);


void __RPC_STUB IAcadLayerStateManager_Export_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAcadLayerStateManager_INTERFACE_DEFINED__ */


#ifndef __IAcadSecurityParams_INTERFACE_DEFINED__
#define __IAcadSecurityParams_INTERFACE_DEFINED__

/* interface IAcadSecurityParams */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IAcadSecurityParams;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6917CED6-6655-475C-B62F-370BE1148408")
    IAcadSecurityParams : public IDispatch
    {
    public:
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Action( 
            /* [in] */ long pOperations) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Action( 
            /* [retval][out] */ long __RPC_FAR *pOperations) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Password( 
            /* [in] */ BSTR pSecret) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Password( 
            /* [retval][out] */ BSTR __RPC_FAR *pSecret) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ProviderType( 
            /* [in] */ long pProvType) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ProviderType( 
            /* [retval][out] */ long __RPC_FAR *pProvType) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ProviderName( 
            /* [in] */ BSTR pProvName) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ProviderName( 
            /* [retval][out] */ BSTR __RPC_FAR *pProvName) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Algorithm( 
            /* [in] */ long pAlgId) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Algorithm( 
            /* [retval][out] */ long __RPC_FAR *pAlgId) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_KeyLength( 
            /* [in] */ long pKeyLen) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_KeyLength( 
            /* [retval][out] */ long __RPC_FAR *pKeyLen) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Subject( 
            /* [in] */ BSTR pCertSubject) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Subject( 
            /* [retval][out] */ BSTR __RPC_FAR *pCertSubject) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Issuer( 
            /* [in] */ BSTR pCertIssuer) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Issuer( 
            /* [retval][out] */ BSTR __RPC_FAR *pCertIssuer) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_SerialNumber( 
            /* [in] */ BSTR pSerialNum) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_SerialNumber( 
            /* [retval][out] */ BSTR __RPC_FAR *pSerialNum) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Comment( 
            /* [in] */ BSTR pText) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Comment( 
            /* [retval][out] */ BSTR __RPC_FAR *pText) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_TimeServer( 
            /* [in] */ BSTR pTimeServerName) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_TimeServer( 
            /* [retval][out] */ BSTR __RPC_FAR *pTimeServerName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAcadSecurityParamsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAcadSecurityParams __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAcadSecurityParams __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Action )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [in] */ long pOperations);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Action )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pOperations);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Password )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [in] */ BSTR pSecret);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Password )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pSecret);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ProviderType )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [in] */ long pProvType);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProviderType )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pProvType);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ProviderName )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [in] */ BSTR pProvName);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProviderName )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pProvName);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Algorithm )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [in] */ long pAlgId);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Algorithm )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pAlgId);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_KeyLength )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [in] */ long pKeyLen);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_KeyLength )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pKeyLen);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Subject )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [in] */ BSTR pCertSubject);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Subject )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pCertSubject);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Issuer )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [in] */ BSTR pCertIssuer);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Issuer )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pCertIssuer);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SerialNumber )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [in] */ BSTR pSerialNum);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SerialNumber )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pSerialNum);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Comment )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [in] */ BSTR pText);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Comment )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pText);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TimeServer )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [in] */ BSTR pTimeServerName);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TimeServer )( 
            IAcadSecurityParams __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pTimeServerName);
        
        END_INTERFACE
    } IAcadSecurityParamsVtbl;

    interface IAcadSecurityParams
    {
        CONST_VTBL struct IAcadSecurityParamsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAcadSecurityParams_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAcadSecurityParams_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAcadSecurityParams_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAcadSecurityParams_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAcadSecurityParams_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAcadSecurityParams_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAcadSecurityParams_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAcadSecurityParams_put_Action(This,pOperations)	\
    (This)->lpVtbl -> put_Action(This,pOperations)

#define IAcadSecurityParams_get_Action(This,pOperations)	\
    (This)->lpVtbl -> get_Action(This,pOperations)

#define IAcadSecurityParams_put_Password(This,pSecret)	\
    (This)->lpVtbl -> put_Password(This,pSecret)

#define IAcadSecurityParams_get_Password(This,pSecret)	\
    (This)->lpVtbl -> get_Password(This,pSecret)

#define IAcadSecurityParams_put_ProviderType(This,pProvType)	\
    (This)->lpVtbl -> put_ProviderType(This,pProvType)

#define IAcadSecurityParams_get_ProviderType(This,pProvType)	\
    (This)->lpVtbl -> get_ProviderType(This,pProvType)

#define IAcadSecurityParams_put_ProviderName(This,pProvName)	\
    (This)->lpVtbl -> put_ProviderName(This,pProvName)

#define IAcadSecurityParams_get_ProviderName(This,pProvName)	\
    (This)->lpVtbl -> get_ProviderName(This,pProvName)

#define IAcadSecurityParams_put_Algorithm(This,pAlgId)	\
    (This)->lpVtbl -> put_Algorithm(This,pAlgId)

#define IAcadSecurityParams_get_Algorithm(This,pAlgId)	\
    (This)->lpVtbl -> get_Algorithm(This,pAlgId)

#define IAcadSecurityParams_put_KeyLength(This,pKeyLen)	\
    (This)->lpVtbl -> put_KeyLength(This,pKeyLen)

#define IAcadSecurityParams_get_KeyLength(This,pKeyLen)	\
    (This)->lpVtbl -> get_KeyLength(This,pKeyLen)

#define IAcadSecurityParams_put_Subject(This,pCertSubject)	\
    (This)->lpVtbl -> put_Subject(This,pCertSubject)

#define IAcadSecurityParams_get_Subject(This,pCertSubject)	\
    (This)->lpVtbl -> get_Subject(This,pCertSubject)

#define IAcadSecurityParams_put_Issuer(This,pCertIssuer)	\
    (This)->lpVtbl -> put_Issuer(This,pCertIssuer)

#define IAcadSecurityParams_get_Issuer(This,pCertIssuer)	\
    (This)->lpVtbl -> get_Issuer(This,pCertIssuer)

#define IAcadSecurityParams_put_SerialNumber(This,pSerialNum)	\
    (This)->lpVtbl -> put_SerialNumber(This,pSerialNum)

#define IAcadSecurityParams_get_SerialNumber(This,pSerialNum)	\
    (This)->lpVtbl -> get_SerialNumber(This,pSerialNum)

#define IAcadSecurityParams_put_Comment(This,pText)	\
    (This)->lpVtbl -> put_Comment(This,pText)

#define IAcadSecurityParams_get_Comment(This,pText)	\
    (This)->lpVtbl -> get_Comment(This,pText)

#define IAcadSecurityParams_put_TimeServer(This,pTimeServerName)	\
    (This)->lpVtbl -> put_TimeServer(This,pTimeServerName)

#define IAcadSecurityParams_get_TimeServer(This,pTimeServerName)	\
    (This)->lpVtbl -> get_TimeServer(This,pTimeServerName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_put_Action_Proxy( 
    IAcadSecurityParams __RPC_FAR * This,
    /* [in] */ long pOperations);


void __RPC_STUB IAcadSecurityParams_put_Action_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_get_Action_Proxy( 
    IAcadSecurityParams __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pOperations);


void __RPC_STUB IAcadSecurityParams_get_Action_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_put_Password_Proxy( 
    IAcadSecurityParams __RPC_FAR * This,
    /* [in] */ BSTR pSecret);


void __RPC_STUB IAcadSecurityParams_put_Password_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_get_Password_Proxy( 
    IAcadSecurityParams __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pSecret);


void __RPC_STUB IAcadSecurityParams_get_Password_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_put_ProviderType_Proxy( 
    IAcadSecurityParams __RPC_FAR * This,
    /* [in] */ long pProvType);


void __RPC_STUB IAcadSecurityParams_put_ProviderType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_get_ProviderType_Proxy( 
    IAcadSecurityParams __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pProvType);


void __RPC_STUB IAcadSecurityParams_get_ProviderType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_put_ProviderName_Proxy( 
    IAcadSecurityParams __RPC_FAR * This,
    /* [in] */ BSTR pProvName);


void __RPC_STUB IAcadSecurityParams_put_ProviderName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_get_ProviderName_Proxy( 
    IAcadSecurityParams __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pProvName);


void __RPC_STUB IAcadSecurityParams_get_ProviderName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_put_Algorithm_Proxy( 
    IAcadSecurityParams __RPC_FAR * This,
    /* [in] */ long pAlgId);


void __RPC_STUB IAcadSecurityParams_put_Algorithm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_get_Algorithm_Proxy( 
    IAcadSecurityParams __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pAlgId);


void __RPC_STUB IAcadSecurityParams_get_Algorithm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_put_KeyLength_Proxy( 
    IAcadSecurityParams __RPC_FAR * This,
    /* [in] */ long pKeyLen);


void __RPC_STUB IAcadSecurityParams_put_KeyLength_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_get_KeyLength_Proxy( 
    IAcadSecurityParams __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pKeyLen);


void __RPC_STUB IAcadSecurityParams_get_KeyLength_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_put_Subject_Proxy( 
    IAcadSecurityParams __RPC_FAR * This,
    /* [in] */ BSTR pCertSubject);


void __RPC_STUB IAcadSecurityParams_put_Subject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_get_Subject_Proxy( 
    IAcadSecurityParams __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pCertSubject);


void __RPC_STUB IAcadSecurityParams_get_Subject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_put_Issuer_Proxy( 
    IAcadSecurityParams __RPC_FAR * This,
    /* [in] */ BSTR pCertIssuer);


void __RPC_STUB IAcadSecurityParams_put_Issuer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_get_Issuer_Proxy( 
    IAcadSecurityParams __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pCertIssuer);


void __RPC_STUB IAcadSecurityParams_get_Issuer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_put_SerialNumber_Proxy( 
    IAcadSecurityParams __RPC_FAR * This,
    /* [in] */ BSTR pSerialNum);


void __RPC_STUB IAcadSecurityParams_put_SerialNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_get_SerialNumber_Proxy( 
    IAcadSecurityParams __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pSerialNum);


void __RPC_STUB IAcadSecurityParams_get_SerialNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_put_Comment_Proxy( 
    IAcadSecurityParams __RPC_FAR * This,
    /* [in] */ BSTR pText);


void __RPC_STUB IAcadSecurityParams_put_Comment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_get_Comment_Proxy( 
    IAcadSecurityParams __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pText);


void __RPC_STUB IAcadSecurityParams_get_Comment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_put_TimeServer_Proxy( 
    IAcadSecurityParams __RPC_FAR * This,
    /* [in] */ BSTR pTimeServerName);


void __RPC_STUB IAcadSecurityParams_put_TimeServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_get_TimeServer_Proxy( 
    IAcadSecurityParams __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pTimeServerName);


void __RPC_STUB IAcadSecurityParams_get_TimeServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAcadSecurityParams_INTERFACE_DEFINED__ */


#ifndef __IOdaSvgExporter_INTERFACE_DEFINED__
#define __IOdaSvgExporter_INTERFACE_DEFINED__

/* interface IOdaSvgExporter */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IOdaSvgExporter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FDAF1115-5A20-4D16-A509-01012B71E4F7")
    IOdaSvgExporter : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [in] */ BSTR Name,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Properties( 
            /* [in] */ BSTR Name,
            /* [in] */ VARIANT NewVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Export( 
            /* [in] */ IAcadDatabase __RPC_FAR *Database,
            /* [in] */ BSTR FilePath) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOdaSvgExporterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IOdaSvgExporter __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IOdaSvgExporter __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IOdaSvgExporter __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IOdaSvgExporter __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IOdaSvgExporter __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IOdaSvgExporter __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IOdaSvgExporter __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            IOdaSvgExporter __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Properties )( 
            IOdaSvgExporter __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [in] */ VARIANT NewVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Export )( 
            IOdaSvgExporter __RPC_FAR * This,
            /* [in] */ IAcadDatabase __RPC_FAR *Database,
            /* [in] */ BSTR FilePath);
        
        END_INTERFACE
    } IOdaSvgExporterVtbl;

    interface IOdaSvgExporter
    {
        CONST_VTBL struct IOdaSvgExporterVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOdaSvgExporter_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOdaSvgExporter_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOdaSvgExporter_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOdaSvgExporter_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IOdaSvgExporter_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IOdaSvgExporter_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IOdaSvgExporter_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IOdaSvgExporter_get_Properties(This,Name,pVal)	\
    (This)->lpVtbl -> get_Properties(This,Name,pVal)

#define IOdaSvgExporter_put_Properties(This,Name,NewVal)	\
    (This)->lpVtbl -> put_Properties(This,Name,NewVal)

#define IOdaSvgExporter_Export(This,Database,FilePath)	\
    (This)->lpVtbl -> Export(This,Database,FilePath)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaSvgExporter_get_Properties_Proxy( 
    IOdaSvgExporter __RPC_FAR * This,
    /* [in] */ BSTR Name,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IOdaSvgExporter_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaSvgExporter_put_Properties_Proxy( 
    IOdaSvgExporter __RPC_FAR * This,
    /* [in] */ BSTR Name,
    /* [in] */ VARIANT NewVal);


void __RPC_STUB IOdaSvgExporter_put_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaSvgExporter_Export_Proxy( 
    IOdaSvgExporter __RPC_FAR * This,
    /* [in] */ IAcadDatabase __RPC_FAR *Database,
    /* [in] */ BSTR FilePath);


void __RPC_STUB IOdaSvgExporter_Export_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOdaSvgExporter_INTERFACE_DEFINED__ */


#ifndef __IOdaAuditInfo_INTERFACE_DEFINED__
#define __IOdaAuditInfo_INTERFACE_DEFINED__

/* interface IOdaAuditInfo */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IOdaAuditInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56B202FE-BE71-43b4-B504-1902EDE89644")
    IOdaAuditInfo : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_numFixes( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_numErrors( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_fixErrors( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOdaAuditInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IOdaAuditInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IOdaAuditInfo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IOdaAuditInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IOdaAuditInfo __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IOdaAuditInfo __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IOdaAuditInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IOdaAuditInfo __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_numFixes )( 
            IOdaAuditInfo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_numErrors )( 
            IOdaAuditInfo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_fixErrors )( 
            IOdaAuditInfo __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        END_INTERFACE
    } IOdaAuditInfoVtbl;

    interface IOdaAuditInfo
    {
        CONST_VTBL struct IOdaAuditInfoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOdaAuditInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOdaAuditInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOdaAuditInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOdaAuditInfo_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IOdaAuditInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IOdaAuditInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IOdaAuditInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IOdaAuditInfo_get_numFixes(This,pVal)	\
    (This)->lpVtbl -> get_numFixes(This,pVal)

#define IOdaAuditInfo_get_numErrors(This,pVal)	\
    (This)->lpVtbl -> get_numErrors(This,pVal)

#define IOdaAuditInfo_get_fixErrors(This,pVal)	\
    (This)->lpVtbl -> get_fixErrors(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaAuditInfo_get_numFixes_Proxy( 
    IOdaAuditInfo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IOdaAuditInfo_get_numFixes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaAuditInfo_get_numErrors_Proxy( 
    IOdaAuditInfo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IOdaAuditInfo_get_numErrors_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaAuditInfo_get_fixErrors_Proxy( 
    IOdaAuditInfo __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IOdaAuditInfo_get_fixErrors_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOdaAuditInfo_INTERFACE_DEFINED__ */


#ifndef ___DAcadApplicationEvents_DISPINTERFACE_DEFINED__
#define ___DAcadApplicationEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DAcadApplicationEvents */
/* [hidden][helpstring][uuid] */ 


EXTERN_C const IID DIID__DAcadApplicationEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("93E291E9-BB72-429b-B4DE-DF76B88603EB")
    _DAcadApplicationEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DAcadApplicationEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _DAcadApplicationEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _DAcadApplicationEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _DAcadApplicationEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _DAcadApplicationEvents __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _DAcadApplicationEvents __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _DAcadApplicationEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _DAcadApplicationEvents __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } _DAcadApplicationEventsVtbl;

    interface _DAcadApplicationEvents
    {
        CONST_VTBL struct _DAcadApplicationEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DAcadApplicationEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _DAcadApplicationEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _DAcadApplicationEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _DAcadApplicationEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _DAcadApplicationEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _DAcadApplicationEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _DAcadApplicationEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DAcadApplicationEvents_DISPINTERFACE_DEFINED__ */


#ifndef ___DAcadDocumentEvents_DISPINTERFACE_DEFINED__
#define ___DAcadDocumentEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DAcadDocumentEvents */
/* [hidden][helpstring][uuid] */ 


EXTERN_C const IID DIID__DAcadDocumentEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("63A90E32-CB8B-4a3b-ADFF-EF94A35278B2")
    _DAcadDocumentEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DAcadDocumentEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _DAcadDocumentEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _DAcadDocumentEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _DAcadDocumentEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _DAcadDocumentEvents __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _DAcadDocumentEvents __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _DAcadDocumentEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _DAcadDocumentEvents __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } _DAcadDocumentEventsVtbl;

    interface _DAcadDocumentEvents
    {
        CONST_VTBL struct _DAcadDocumentEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DAcadDocumentEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _DAcadDocumentEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _DAcadDocumentEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _DAcadDocumentEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _DAcadDocumentEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _DAcadDocumentEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _DAcadDocumentEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DAcadDocumentEvents_DISPINTERFACE_DEFINED__ */


#ifndef __IOdaHostApp_INTERFACE_DEFINED__
#define __IOdaHostApp_INTERFACE_DEFINED__

/* interface IOdaHostApp */
/* [oleautomation][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IOdaHostApp;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E789E253-BEDC-44ff-B228-AD3A17D9AB3A")
    IOdaHostApp : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IAcadApplication __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE BrowseForOpenFileName( 
            BSTR rootFolder,
            BSTR fileFilter,
            VARIANT_BOOL __RPC_FAR *bOk,
            BSTR __RPC_FAR *filename) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AuditDatabase( 
            IAcadDocument __RPC_FAR *pDoc,
            VARIANT_BOOL bFixErrors) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AuditInfo( 
            /* [retval][out] */ IOdaAuditInfo __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OriginalFileType( 
            IAcadDocument __RPC_FAR *pDoc,
            VARIANT __RPC_FAR *FileType) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Recover( 
            /* [in] */ BSTR fileName,
            /* [optional][in] */ VARIANT Password,
            /* [retval][out] */ IAcadDocument __RPC_FAR *__RPC_FAR *pDoc) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OpenDbPartially( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OpenDbPartially( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOdaHostAppVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IOdaHostApp __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IOdaHostApp __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IOdaHostApp __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IOdaHostApp __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IOdaHostApp __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IOdaHostApp __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IOdaHostApp __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Application )( 
            IOdaHostApp __RPC_FAR * This,
            /* [retval][out] */ IAcadApplication __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BrowseForOpenFileName )( 
            IOdaHostApp __RPC_FAR * This,
            BSTR rootFolder,
            BSTR fileFilter,
            VARIANT_BOOL __RPC_FAR *bOk,
            BSTR __RPC_FAR *filename);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AuditDatabase )( 
            IOdaHostApp __RPC_FAR * This,
            IAcadDocument __RPC_FAR *pDoc,
            VARIANT_BOOL bFixErrors);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AuditInfo )( 
            IOdaHostApp __RPC_FAR * This,
            /* [retval][out] */ IOdaAuditInfo __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OriginalFileType )( 
            IOdaHostApp __RPC_FAR * This,
            IAcadDocument __RPC_FAR *pDoc,
            VARIANT __RPC_FAR *FileType);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Recover )( 
            IOdaHostApp __RPC_FAR * This,
            /* [in] */ BSTR fileName,
            /* [optional][in] */ VARIANT Password,
            /* [retval][out] */ IAcadDocument __RPC_FAR *__RPC_FAR *pDoc);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OpenDbPartially )( 
            IOdaHostApp __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OpenDbPartially )( 
            IOdaHostApp __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        END_INTERFACE
    } IOdaHostAppVtbl;

    interface IOdaHostApp
    {
        CONST_VTBL struct IOdaHostAppVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOdaHostApp_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOdaHostApp_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOdaHostApp_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOdaHostApp_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IOdaHostApp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IOdaHostApp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IOdaHostApp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IOdaHostApp_get_Application(This,pVal)	\
    (This)->lpVtbl -> get_Application(This,pVal)

#define IOdaHostApp_BrowseForOpenFileName(This,rootFolder,fileFilter,bOk,filename)	\
    (This)->lpVtbl -> BrowseForOpenFileName(This,rootFolder,fileFilter,bOk,filename)

#define IOdaHostApp_AuditDatabase(This,pDoc,bFixErrors)	\
    (This)->lpVtbl -> AuditDatabase(This,pDoc,bFixErrors)

#define IOdaHostApp_get_AuditInfo(This,pVal)	\
    (This)->lpVtbl -> get_AuditInfo(This,pVal)

#define IOdaHostApp_OriginalFileType(This,pDoc,FileType)	\
    (This)->lpVtbl -> OriginalFileType(This,pDoc,FileType)

#define IOdaHostApp_Recover(This,fileName,Password,pDoc)	\
    (This)->lpVtbl -> Recover(This,fileName,Password,pDoc)

#define IOdaHostApp_get_OpenDbPartially(This,pVal)	\
    (This)->lpVtbl -> get_OpenDbPartially(This,pVal)

#define IOdaHostApp_put_OpenDbPartially(This,newVal)	\
    (This)->lpVtbl -> put_OpenDbPartially(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaHostApp_get_Application_Proxy( 
    IOdaHostApp __RPC_FAR * This,
    /* [retval][out] */ IAcadApplication __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IOdaHostApp_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaHostApp_BrowseForOpenFileName_Proxy( 
    IOdaHostApp __RPC_FAR * This,
    BSTR rootFolder,
    BSTR fileFilter,
    VARIANT_BOOL __RPC_FAR *bOk,
    BSTR __RPC_FAR *filename);


void __RPC_STUB IOdaHostApp_BrowseForOpenFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaHostApp_AuditDatabase_Proxy( 
    IOdaHostApp __RPC_FAR * This,
    IAcadDocument __RPC_FAR *pDoc,
    VARIANT_BOOL bFixErrors);


void __RPC_STUB IOdaHostApp_AuditDatabase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaHostApp_get_AuditInfo_Proxy( 
    IOdaHostApp __RPC_FAR * This,
    /* [retval][out] */ IOdaAuditInfo __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IOdaHostApp_get_AuditInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaHostApp_OriginalFileType_Proxy( 
    IOdaHostApp __RPC_FAR * This,
    IAcadDocument __RPC_FAR *pDoc,
    VARIANT __RPC_FAR *FileType);


void __RPC_STUB IOdaHostApp_OriginalFileType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaHostApp_Recover_Proxy( 
    IOdaHostApp __RPC_FAR * This,
    /* [in] */ BSTR fileName,
    /* [optional][in] */ VARIANT Password,
    /* [retval][out] */ IAcadDocument __RPC_FAR *__RPC_FAR *pDoc);


void __RPC_STUB IOdaHostApp_Recover_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaHostApp_get_OpenDbPartially_Proxy( 
    IOdaHostApp __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IOdaHostApp_get_OpenDbPartially_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaHostApp_put_OpenDbPartially_Proxy( 
    IOdaHostApp __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IOdaHostApp_put_OpenDbPartially_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOdaHostApp_INTERFACE_DEFINED__ */


#ifndef __IOdaDwfPageData_INTERFACE_DEFINED__
#define __IOdaDwfPageData_INTERFACE_DEFINED__

/* interface IOdaDwfPageData */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IOdaDwfPageData;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6C2751B5-A21F-46d8-8EBE-59F0A15288D9")
    IOdaDwfPageData : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LayoutName( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Author( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Author( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Title( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Title( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Subject( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Subject( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Company( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Company( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Comments( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Comments( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Reviewers( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Reviewers( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Keywords( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Keywords( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Description( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Copyright( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Copyright( 
            /* [in] */ BSTR newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOdaDwfPageDataVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IOdaDwfPageData __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IOdaDwfPageData __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IOdaDwfPageData __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IOdaDwfPageData __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IOdaDwfPageData __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IOdaDwfPageData __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IOdaDwfPageData __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LayoutName )( 
            IOdaDwfPageData __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Author )( 
            IOdaDwfPageData __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Author )( 
            IOdaDwfPageData __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Title )( 
            IOdaDwfPageData __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Title )( 
            IOdaDwfPageData __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Subject )( 
            IOdaDwfPageData __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Subject )( 
            IOdaDwfPageData __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Company )( 
            IOdaDwfPageData __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Company )( 
            IOdaDwfPageData __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Comments )( 
            IOdaDwfPageData __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Comments )( 
            IOdaDwfPageData __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Reviewers )( 
            IOdaDwfPageData __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Reviewers )( 
            IOdaDwfPageData __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Keywords )( 
            IOdaDwfPageData __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Keywords )( 
            IOdaDwfPageData __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Description )( 
            IOdaDwfPageData __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Description )( 
            IOdaDwfPageData __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Copyright )( 
            IOdaDwfPageData __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Copyright )( 
            IOdaDwfPageData __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        END_INTERFACE
    } IOdaDwfPageDataVtbl;

    interface IOdaDwfPageData
    {
        CONST_VTBL struct IOdaDwfPageDataVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOdaDwfPageData_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOdaDwfPageData_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOdaDwfPageData_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOdaDwfPageData_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IOdaDwfPageData_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IOdaDwfPageData_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IOdaDwfPageData_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IOdaDwfPageData_get_LayoutName(This,pVal)	\
    (This)->lpVtbl -> get_LayoutName(This,pVal)

#define IOdaDwfPageData_get_Author(This,pVal)	\
    (This)->lpVtbl -> get_Author(This,pVal)

#define IOdaDwfPageData_put_Author(This,newVal)	\
    (This)->lpVtbl -> put_Author(This,newVal)

#define IOdaDwfPageData_get_Title(This,pVal)	\
    (This)->lpVtbl -> get_Title(This,pVal)

#define IOdaDwfPageData_put_Title(This,newVal)	\
    (This)->lpVtbl -> put_Title(This,newVal)

#define IOdaDwfPageData_get_Subject(This,pVal)	\
    (This)->lpVtbl -> get_Subject(This,pVal)

#define IOdaDwfPageData_put_Subject(This,newVal)	\
    (This)->lpVtbl -> put_Subject(This,newVal)

#define IOdaDwfPageData_get_Company(This,pVal)	\
    (This)->lpVtbl -> get_Company(This,pVal)

#define IOdaDwfPageData_put_Company(This,newVal)	\
    (This)->lpVtbl -> put_Company(This,newVal)

#define IOdaDwfPageData_get_Comments(This,pVal)	\
    (This)->lpVtbl -> get_Comments(This,pVal)

#define IOdaDwfPageData_put_Comments(This,newVal)	\
    (This)->lpVtbl -> put_Comments(This,newVal)

#define IOdaDwfPageData_get_Reviewers(This,pVal)	\
    (This)->lpVtbl -> get_Reviewers(This,pVal)

#define IOdaDwfPageData_put_Reviewers(This,newVal)	\
    (This)->lpVtbl -> put_Reviewers(This,newVal)

#define IOdaDwfPageData_get_Keywords(This,pVal)	\
    (This)->lpVtbl -> get_Keywords(This,pVal)

#define IOdaDwfPageData_put_Keywords(This,newVal)	\
    (This)->lpVtbl -> put_Keywords(This,newVal)

#define IOdaDwfPageData_get_Description(This,pVal)	\
    (This)->lpVtbl -> get_Description(This,pVal)

#define IOdaDwfPageData_put_Description(This,newVal)	\
    (This)->lpVtbl -> put_Description(This,newVal)

#define IOdaDwfPageData_get_Copyright(This,pVal)	\
    (This)->lpVtbl -> get_Copyright(This,pVal)

#define IOdaDwfPageData_put_Copyright(This,newVal)	\
    (This)->lpVtbl -> put_Copyright(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_get_LayoutName_Proxy( 
    IOdaDwfPageData __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOdaDwfPageData_get_LayoutName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_get_Author_Proxy( 
    IOdaDwfPageData __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOdaDwfPageData_get_Author_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_put_Author_Proxy( 
    IOdaDwfPageData __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfPageData_put_Author_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_get_Title_Proxy( 
    IOdaDwfPageData __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOdaDwfPageData_get_Title_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_put_Title_Proxy( 
    IOdaDwfPageData __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfPageData_put_Title_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_get_Subject_Proxy( 
    IOdaDwfPageData __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOdaDwfPageData_get_Subject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_put_Subject_Proxy( 
    IOdaDwfPageData __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfPageData_put_Subject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_get_Company_Proxy( 
    IOdaDwfPageData __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOdaDwfPageData_get_Company_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_put_Company_Proxy( 
    IOdaDwfPageData __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfPageData_put_Company_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_get_Comments_Proxy( 
    IOdaDwfPageData __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOdaDwfPageData_get_Comments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_put_Comments_Proxy( 
    IOdaDwfPageData __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfPageData_put_Comments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_get_Reviewers_Proxy( 
    IOdaDwfPageData __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOdaDwfPageData_get_Reviewers_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_put_Reviewers_Proxy( 
    IOdaDwfPageData __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfPageData_put_Reviewers_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_get_Keywords_Proxy( 
    IOdaDwfPageData __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOdaDwfPageData_get_Keywords_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_put_Keywords_Proxy( 
    IOdaDwfPageData __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfPageData_put_Keywords_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_get_Description_Proxy( 
    IOdaDwfPageData __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOdaDwfPageData_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_put_Description_Proxy( 
    IOdaDwfPageData __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfPageData_put_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_get_Copyright_Proxy( 
    IOdaDwfPageData __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOdaDwfPageData_get_Copyright_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_put_Copyright_Proxy( 
    IOdaDwfPageData __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfPageData_put_Copyright_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOdaDwfPageData_INTERFACE_DEFINED__ */


#ifndef __IOdaDwfTemplate_INTERFACE_DEFINED__
#define __IOdaDwfTemplate_INTERFACE_DEFINED__

/* interface IOdaDwfTemplate */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IOdaDwfTemplate;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8F2A46C2-4D8A-46D5-8BBE-B06A4027BE10")
    IOdaDwfTemplate : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteFile( 
            /* [in] */ IAcadDatabase __RPC_FAR *Database,
            /* [in] */ BSTR FileName,
            /* [in] */ OdDwfFormat FileType,
            /* [in] */ OdDwfVersion Version) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddPage( 
            /* [in] */ BSTR LayoutName,
            /* [retval][out] */ long __RPC_FAR *PageIndex) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PageCount( 
            /* [retval][out] */ long __RPC_FAR *pCount) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Page( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IOdaDwfPageData __RPC_FAR *__RPC_FAR *PageData) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemovePage( 
            /* [in] */ VARIANT Index) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemoveAllPages( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Background( 
            /* [retval][out] */ IAcadAcCmColor __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Background( 
            /* [in] */ IAcadAcCmColor __RPC_FAR *newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ExportInvisibleLayers( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ExportInvisibleLayers( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ForceInitialViewToExtents( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ForceInitialViewToExtents( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SkipLayerInfo( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SkipLayerInfo( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SkipNamedViewsInfo( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SkipNamedViewsInfo( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Password( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Password( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Publisher( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Publisher( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_WideComments( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_WideComments( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SourceProductName( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SourceProductName( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Palette( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Palette( 
            /* [in] */ VARIANT pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_xSize( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_xSize( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ySize( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ySize( 
            /* [in] */ long newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOdaDwfTemplateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IOdaDwfTemplate __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IOdaDwfTemplate __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WriteFile )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [in] */ IAcadDatabase __RPC_FAR *Database,
            /* [in] */ BSTR FileName,
            /* [in] */ OdDwfFormat FileType,
            /* [in] */ OdDwfVersion Version);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddPage )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [in] */ BSTR LayoutName,
            /* [retval][out] */ long __RPC_FAR *PageIndex);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PageCount )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCount);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Page )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IOdaDwfPageData __RPC_FAR *__RPC_FAR *PageData);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RemovePage )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [in] */ VARIANT Index);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RemoveAllPages )( 
            IOdaDwfTemplate __RPC_FAR * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Background )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [retval][out] */ IAcadAcCmColor __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Background )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [in] */ IAcadAcCmColor __RPC_FAR *newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ExportInvisibleLayers )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ExportInvisibleLayers )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ForceInitialViewToExtents )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ForceInitialViewToExtents )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SkipLayerInfo )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SkipLayerInfo )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SkipNamedViewsInfo )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SkipNamedViewsInfo )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Password )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Password )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Publisher )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Publisher )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WideComments )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WideComments )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceProductName )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SourceProductName )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Palette )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Palette )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [in] */ VARIANT pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_xSize )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_xSize )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ySize )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ySize )( 
            IOdaDwfTemplate __RPC_FAR * This,
            /* [in] */ long newVal);
        
        END_INTERFACE
    } IOdaDwfTemplateVtbl;

    interface IOdaDwfTemplate
    {
        CONST_VTBL struct IOdaDwfTemplateVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOdaDwfTemplate_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOdaDwfTemplate_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOdaDwfTemplate_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOdaDwfTemplate_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IOdaDwfTemplate_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IOdaDwfTemplate_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IOdaDwfTemplate_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IOdaDwfTemplate_WriteFile(This,Database,FileName,FileType,Version)	\
    (This)->lpVtbl -> WriteFile(This,Database,FileName,FileType,Version)

#define IOdaDwfTemplate_AddPage(This,LayoutName,PageIndex)	\
    (This)->lpVtbl -> AddPage(This,LayoutName,PageIndex)

#define IOdaDwfTemplate_get_PageCount(This,pCount)	\
    (This)->lpVtbl -> get_PageCount(This,pCount)

#define IOdaDwfTemplate_get_Page(This,Index,PageData)	\
    (This)->lpVtbl -> get_Page(This,Index,PageData)

#define IOdaDwfTemplate_RemovePage(This,Index)	\
    (This)->lpVtbl -> RemovePage(This,Index)

#define IOdaDwfTemplate_RemoveAllPages(This)	\
    (This)->lpVtbl -> RemoveAllPages(This)

#define IOdaDwfTemplate_get_Background(This,pVal)	\
    (This)->lpVtbl -> get_Background(This,pVal)

#define IOdaDwfTemplate_put_Background(This,newVal)	\
    (This)->lpVtbl -> put_Background(This,newVal)

#define IOdaDwfTemplate_get_ExportInvisibleLayers(This,pVal)	\
    (This)->lpVtbl -> get_ExportInvisibleLayers(This,pVal)

#define IOdaDwfTemplate_put_ExportInvisibleLayers(This,newVal)	\
    (This)->lpVtbl -> put_ExportInvisibleLayers(This,newVal)

#define IOdaDwfTemplate_get_ForceInitialViewToExtents(This,pVal)	\
    (This)->lpVtbl -> get_ForceInitialViewToExtents(This,pVal)

#define IOdaDwfTemplate_put_ForceInitialViewToExtents(This,newVal)	\
    (This)->lpVtbl -> put_ForceInitialViewToExtents(This,newVal)

#define IOdaDwfTemplate_get_SkipLayerInfo(This,pVal)	\
    (This)->lpVtbl -> get_SkipLayerInfo(This,pVal)

#define IOdaDwfTemplate_put_SkipLayerInfo(This,newVal)	\
    (This)->lpVtbl -> put_SkipLayerInfo(This,newVal)

#define IOdaDwfTemplate_get_SkipNamedViewsInfo(This,pVal)	\
    (This)->lpVtbl -> get_SkipNamedViewsInfo(This,pVal)

#define IOdaDwfTemplate_put_SkipNamedViewsInfo(This,newVal)	\
    (This)->lpVtbl -> put_SkipNamedViewsInfo(This,newVal)

#define IOdaDwfTemplate_get_Password(This,pVal)	\
    (This)->lpVtbl -> get_Password(This,pVal)

#define IOdaDwfTemplate_put_Password(This,newVal)	\
    (This)->lpVtbl -> put_Password(This,newVal)

#define IOdaDwfTemplate_get_Publisher(This,pVal)	\
    (This)->lpVtbl -> get_Publisher(This,pVal)

#define IOdaDwfTemplate_put_Publisher(This,newVal)	\
    (This)->lpVtbl -> put_Publisher(This,newVal)

#define IOdaDwfTemplate_get_WideComments(This,pVal)	\
    (This)->lpVtbl -> get_WideComments(This,pVal)

#define IOdaDwfTemplate_put_WideComments(This,newVal)	\
    (This)->lpVtbl -> put_WideComments(This,newVal)

#define IOdaDwfTemplate_get_SourceProductName(This,pVal)	\
    (This)->lpVtbl -> get_SourceProductName(This,pVal)

#define IOdaDwfTemplate_put_SourceProductName(This,newVal)	\
    (This)->lpVtbl -> put_SourceProductName(This,newVal)

#define IOdaDwfTemplate_get_Palette(This,pVal)	\
    (This)->lpVtbl -> get_Palette(This,pVal)

#define IOdaDwfTemplate_put_Palette(This,pVal)	\
    (This)->lpVtbl -> put_Palette(This,pVal)

#define IOdaDwfTemplate_get_xSize(This,pVal)	\
    (This)->lpVtbl -> get_xSize(This,pVal)

#define IOdaDwfTemplate_put_xSize(This,newVal)	\
    (This)->lpVtbl -> put_xSize(This,newVal)

#define IOdaDwfTemplate_get_ySize(This,pVal)	\
    (This)->lpVtbl -> get_ySize(This,pVal)

#define IOdaDwfTemplate_put_ySize(This,newVal)	\
    (This)->lpVtbl -> put_ySize(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_WriteFile_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [in] */ IAcadDatabase __RPC_FAR *Database,
    /* [in] */ BSTR FileName,
    /* [in] */ OdDwfFormat FileType,
    /* [in] */ OdDwfVersion Version);


void __RPC_STUB IOdaDwfTemplate_WriteFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_AddPage_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [in] */ BSTR LayoutName,
    /* [retval][out] */ long __RPC_FAR *PageIndex);


void __RPC_STUB IOdaDwfTemplate_AddPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_PageCount_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCount);


void __RPC_STUB IOdaDwfTemplate_get_PageCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_Page_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ IOdaDwfPageData __RPC_FAR *__RPC_FAR *PageData);


void __RPC_STUB IOdaDwfTemplate_get_Page_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_RemovePage_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [in] */ VARIANT Index);


void __RPC_STUB IOdaDwfTemplate_RemovePage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_RemoveAllPages_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This);


void __RPC_STUB IOdaDwfTemplate_RemoveAllPages_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_Background_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [retval][out] */ IAcadAcCmColor __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IOdaDwfTemplate_get_Background_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_put_Background_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [in] */ IAcadAcCmColor __RPC_FAR *newVal);


void __RPC_STUB IOdaDwfTemplate_put_Background_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_ExportInvisibleLayers_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IOdaDwfTemplate_get_ExportInvisibleLayers_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_put_ExportInvisibleLayers_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IOdaDwfTemplate_put_ExportInvisibleLayers_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_ForceInitialViewToExtents_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IOdaDwfTemplate_get_ForceInitialViewToExtents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_put_ForceInitialViewToExtents_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IOdaDwfTemplate_put_ForceInitialViewToExtents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_SkipLayerInfo_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IOdaDwfTemplate_get_SkipLayerInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_put_SkipLayerInfo_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IOdaDwfTemplate_put_SkipLayerInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_SkipNamedViewsInfo_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IOdaDwfTemplate_get_SkipNamedViewsInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_put_SkipNamedViewsInfo_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IOdaDwfTemplate_put_SkipNamedViewsInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_Password_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOdaDwfTemplate_get_Password_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_put_Password_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfTemplate_put_Password_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_Publisher_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOdaDwfTemplate_get_Publisher_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_put_Publisher_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfTemplate_put_Publisher_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_WideComments_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOdaDwfTemplate_get_WideComments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_put_WideComments_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfTemplate_put_WideComments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_SourceProductName_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOdaDwfTemplate_get_SourceProductName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_put_SourceProductName_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfTemplate_put_SourceProductName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_Palette_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IOdaDwfTemplate_get_Palette_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_put_Palette_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [in] */ VARIANT pVal);


void __RPC_STUB IOdaDwfTemplate_put_Palette_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_xSize_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IOdaDwfTemplate_get_xSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_put_xSize_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IOdaDwfTemplate_put_xSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_ySize_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IOdaDwfTemplate_get_ySize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_put_ySize_Proxy( 
    IOdaDwfTemplate __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IOdaDwfTemplate_put_ySize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOdaDwfTemplate_INTERFACE_DEFINED__ */


#ifndef __IOdaDwfImporter_INTERFACE_DEFINED__
#define __IOdaDwfImporter_INTERFACE_DEFINED__

/* interface IOdaDwfImporter */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IOdaDwfImporter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C5631279-833D-4fe0-8965-B06E2FEC0E9C")
    IOdaDwfImporter : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Import( 
            /* [in] */ IAcadDatabase __RPC_FAR *Database,
            /* [in] */ BSTR FileName,
            /* [defaultvalue][optional][in] */ BSTR password = L"",
            /* [defaultvalue][optional][in] */ double wPaper = 297,
            /* [defaultvalue][optional][in] */ double hPaper = 210) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOdaDwfImporterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IOdaDwfImporter __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IOdaDwfImporter __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IOdaDwfImporter __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IOdaDwfImporter __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IOdaDwfImporter __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IOdaDwfImporter __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IOdaDwfImporter __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Import )( 
            IOdaDwfImporter __RPC_FAR * This,
            /* [in] */ IAcadDatabase __RPC_FAR *Database,
            /* [in] */ BSTR FileName,
            /* [defaultvalue][optional][in] */ BSTR password,
            /* [defaultvalue][optional][in] */ double wPaper,
            /* [defaultvalue][optional][in] */ double hPaper);
        
        END_INTERFACE
    } IOdaDwfImporterVtbl;

    interface IOdaDwfImporter
    {
        CONST_VTBL struct IOdaDwfImporterVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOdaDwfImporter_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOdaDwfImporter_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOdaDwfImporter_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOdaDwfImporter_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IOdaDwfImporter_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IOdaDwfImporter_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IOdaDwfImporter_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IOdaDwfImporter_Import(This,Database,FileName,password,wPaper,hPaper)	\
    (This)->lpVtbl -> Import(This,Database,FileName,password,wPaper,hPaper)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaDwfImporter_Import_Proxy( 
    IOdaDwfImporter __RPC_FAR * This,
    /* [in] */ IAcadDatabase __RPC_FAR *Database,
    /* [in] */ BSTR FileName,
    /* [defaultvalue][optional][in] */ BSTR password,
    /* [defaultvalue][optional][in] */ double wPaper,
    /* [defaultvalue][optional][in] */ double hPaper);


void __RPC_STUB IOdaDwfImporter_Import_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOdaDwfImporter_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_AcadApplication;

#ifdef __cplusplus

class DECLSPEC_UUID("4858f12f-d346-4c75-b6e4-6586808cfc1e")
AcadApplication;
#endif

EXTERN_C const CLSID CLSID_AcadDocument;

#ifdef __cplusplus

class DECLSPEC_UUID("c5a278a0-66c0-4f46-9b59-1cffc154286a")
AcadDocument;
#endif

#ifndef ___IOdaHostAppEvents_DISPINTERFACE_DEFINED__
#define ___IOdaHostAppEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IOdaHostAppEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IOdaHostAppEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("5FA75E80-D112-4186-AD8E-0992D7F13475")
    _IOdaHostAppEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IOdaHostAppEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _IOdaHostAppEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _IOdaHostAppEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _IOdaHostAppEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _IOdaHostAppEvents __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _IOdaHostAppEvents __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _IOdaHostAppEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _IOdaHostAppEvents __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } _IOdaHostAppEventsVtbl;

    interface _IOdaHostAppEvents
    {
        CONST_VTBL struct _IOdaHostAppEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IOdaHostAppEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IOdaHostAppEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IOdaHostAppEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IOdaHostAppEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IOdaHostAppEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IOdaHostAppEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IOdaHostAppEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IOdaHostAppEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_OdaHostApp;

#ifdef __cplusplus

class DECLSPEC_UUID("30c9b3b6-d924-40a1-963e-2192a9d7faa1")
OdaHostApp;
#endif

EXTERN_C const CLSID CLSID_AcadUtility;

#ifdef __cplusplus

class DECLSPEC_UUID("b57d95ce-783a-4834-8527-036561a6a3c7")
AcadUtility;
#endif

EXTERN_C const CLSID CLSID_AcadPreferences;

#ifdef __cplusplus

class DECLSPEC_UUID("eb128720-43f5-4cd6-9588-2a5c39331b6f")
AcadPreferences;
#endif

EXTERN_C const CLSID CLSID_AcadSelectionSet;

#ifdef __cplusplus

class DECLSPEC_UUID("d4d62467-3eaf-47f6-a858-58e4f1e879e7")
AcadSelectionSet;
#endif

EXTERN_C const CLSID CLSID_OdaDwfTemplate;

#ifdef __cplusplus

class DECLSPEC_UUID("c03b6059-883c-4a41-bad0-fa2c3956cf2d")
OdaDwfTemplate;
#endif

EXTERN_C const CLSID CLSID_OdaDwfImporter;

#ifdef __cplusplus

class DECLSPEC_UUID("78901937-7dbd-4366-85a1-a68e4fe6d1b8")
OdaDwfImporter;
#endif

EXTERN_C const CLSID CLSID_AcadLayerStateManager;

#ifdef __cplusplus

class DECLSPEC_UUID("714e87e7-2b03-47ee-bf28-b22c9498e2a7")
AcadLayerStateManager;
#endif

EXTERN_C const CLSID CLSID_AcadSecurityParams;

#ifdef __cplusplus

class DECLSPEC_UUID("b0e92a32-0e54-4113-8765-5567daab2291")
AcadSecurityParams;
#endif

EXTERN_C const CLSID CLSID_OdaSvgExporter;

#ifdef __cplusplus

class DECLSPEC_UUID("928644ec-aae5-493c-88cc-e0b42bb0f97c")
OdaSvgExporter;
#endif
#endif /* __DWGdirectX_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
