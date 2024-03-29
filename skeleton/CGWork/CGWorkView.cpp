// CGWorkView.cpp : implementation of the CCGWorkView class
//
#include "stdafx.h"
#include "CGWork.h"

#include "CGWorkDoc.h"
#include "CGWorkView.h"

#include <iostream>
using std::cout;
using std::endl;
#include "MaterialDlg.h"
#include "LightDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "PngWrapper.h"
#include "iritSkel.h"


// For Status Bar access
#include "MainFrm.h"

#include <vector>
#include "Scene.h"

// Use this macro to display text messages in the status bar.
#define STATUS_BAR_TEXT(str) (((CMainFrame*)GetParentFrame())->getStatusBar().SetWindowText(str))


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView



Scene scene;


IMPLEMENT_DYNCREATE(CCGWorkView, CView)

BEGIN_MESSAGE_MAP(CCGWorkView, CView)
	//{{AFX_MSG_MAP(CCGWorkView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_LOAD, OnFileLoad)

	ON_COMMAND(ID_VIEW_ORTHOGRAPHIC, OnViewOrthographic)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ORTHOGRAPHIC, OnUpdateViewOrthographic)
	ON_COMMAND(ID_VIEW_PERSPECTIVE, OnViewPerspective)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PERSPECTIVE, OnUpdateViewPerspective)
	ON_COMMAND(ID_ACTION_ROTATE, OnActionRotate)
	ON_UPDATE_COMMAND_UI(ID_ACTION_ROTATE, OnUpdateActionRotate)
	ON_COMMAND(ID_ACTION_SCALE, OnActionScale)
	ON_UPDATE_COMMAND_UI(ID_ACTION_SCALE, OnUpdateActionScale)
	ON_COMMAND(ID_ACTION_TRANSLATE, OnActionTranslate)
	ON_UPDATE_COMMAND_UI(ID_ACTION_TRANSLATE, OnUpdateActionTranslate)
	ON_COMMAND(ID_AXIS_X, OnAxisX)
	ON_UPDATE_COMMAND_UI(ID_AXIS_X, OnUpdateAxisX)
	ON_COMMAND(ID_AXIS_Y, OnAxisY)
	ON_UPDATE_COMMAND_UI(ID_AXIS_Y, OnUpdateAxisY)
	ON_COMMAND(ID_AXIS_Z, OnAxisZ)
	ON_UPDATE_COMMAND_UI(ID_AXIS_Z, OnUpdateAxisZ)

	// face normals
	ON_COMMAND(ID_FACE_NORMALS, OnFaceNormals)
	ON_UPDATE_COMMAND_UI(ID_FACE_NORMALS, OnUpdateFaceNormals)
	ON_COMMAND(ID_FACE_NORMALS_CALCULATED, OnFaceNormalsCalc)
	ON_UPDATE_COMMAND_UI(ID_FACE_NORMALS_CALCULATED, OnUpdateFaceNormalsCalc)

	//ver normals
	ON_COMMAND(ID_SHOW_VER_NORMALS, OnVerNormals)
	ON_UPDATE_COMMAND_UI(ID_SHOW_VER_NORMALS, OnUpdateVerNormals)
	ON_COMMAND(ID_VERTEX_NORMALS_CALCULATED, OnVerNormalsCalc)
	ON_UPDATE_COMMAND_UI(ID_VERTEX_NORMALS_CALCULATED, OnUpdateVerNormalsCalc)
	//bounding box
	ON_COMMAND(ID_SHOW_BOUNDING_BOX, OnShowBoundingBox)
	ON_UPDATE_COMMAND_UI(ID_SHOW_BOUNDING_BOX, OnUpdateShowBoundingBox)



	//ID_VIEW_FACENORMALS






	ON_COMMAND(ID_LIGHT_SHADING_FLAT, OnLightShadingFlat)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_SHADING_FLAT, OnUpdateLightShadingFlat)
	ON_COMMAND(ID_LIGHT_SHADING_GOURAUD, OnLightShadingGouraud)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_SHADING_GOURAUD, OnUpdateLightShadingGouraud)
	ON_COMMAND(ID_LIGHT_CONSTANTS, OnLightConstants)
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()



	ON_COMMAND(ID_COORDINATESYSTEM_VIEW, &CCGWorkView::OnCoordinatesystemView)
	ON_UPDATE_COMMAND_UI(ID_COORDINATESYSTEM_VIEW, &CCGWorkView::OnUpdateCoordinatesystemView)
	ON_COMMAND(ID_COORDINATESYSTEM_MODEL, &CCGWorkView::OnCoordinatesystemModel)
	ON_UPDATE_COMMAND_UI(ID_COORDINATESYSTEM_MODEL, &CCGWorkView::OnUpdateCoordinatesystemModel)
	ON_COMMAND(ID_COLORS_WIREFRAMES, &CCGWorkView::OnColorsWireFrame)
	ON_COMMAND(ID_COLORS_BACKGROUND, &CCGWorkView::OnColorsBackground)
	ON_COMMAND(ID_COLORS_FACENORMALS, &CCGWorkView::OnColorsFaceNormals)
	ON_COMMAND(ID_COLORS_VERTICESNORMALS, &CCGWorkView::OnColorsVerticesNormals)
	ON_COMMAND(ID_COLORS_BOUNDINGBOX, &CCGWorkView::OnColorsBoundingBox)


	ON_COMMAND(ID_OPTIONS_MOUSESENSITIVITY, &CCGWorkView::OnOptionsMousesensitivity)
	ON_COMMAND(ID_OPTIONS_PERSPECTIVECONTROL, &CCGWorkView::OnOptionsPerspectivecontrol)
	ON_COMMAND(ID_OPTIONS_POLYGONFINENESSTOLERANCE, &CCGWorkView::OnOptionsPolygonfinenesstolerance)
	ON_COMMAND(ID_ACTIVE_MESH_CONTROL ,&CCGWorkView::OnOptionsActiveMesh )
	
END_MESSAGE_MAP()


// A patch to fix GLaux disappearance from VS2005 to VS2008
void auxSolidCone(GLdouble radius, GLdouble height) {
        GLUquadric *quad = gluNewQuadric();
        gluQuadricDrawStyle(quad, GLU_FILL);
        gluCylinder(quad, radius, 0.0, height, 20, 20);
        gluDeleteQuadric(quad);
}

/////////////////////////////////////////////////////////////////////////////
// CCGWorkView construction/destruction

CCGWorkView::CCGWorkView() :
	_mouse_sensetivity_dialog(this),
	_perspective_dialog(this),
	_polygon_dialog(this),
	_active_mesh_dialog(this)
{
	// Set default values
	m_nAxis = ID_AXIS_X;
	m_nAction = ID_ACTION_ROTATE;
	m_nView = ID_VIEW_ORTHOGRAPHIC;	
	m_bIsPerspective = false;

	m_nLightShading = ID_LIGHT_SHADING_FLAT;

	m_lMaterialAmbient = 0.2;
	m_lMaterialDiffuse = 0.8;
	m_lMaterialSpecular = 1.0;
	m_nMaterialCosineFactor = 32;

	//init the first light to be enabled
	m_lights[LIGHT_ID_1].enabled=true;
	m_pDbBitMap = NULL;
	m_pDbDC = NULL;

	_curr_coordinate_system = CoordinateSystem::VIEW;
	m_RotationSensitivity = _mouse_sensetivity_dialog.getRotationSensetivity();
	m_TranslationSensitivity = _mouse_sensetivity_dialog.getTranslationSensetivity();
	m_ScaleSensitivity = _mouse_sensetivity_dialog.getScaleSensetivity();
}

CCGWorkView::~CCGWorkView()
{
}


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView diagnostics

#ifdef _DEBUG
void CCGWorkView::AssertValid() const
{
	CView::AssertValid();
}

void CCGWorkView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCGWorkDoc* CCGWorkView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCGWorkDoc)));
	return (CCGWorkDoc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView Window Creation - Linkage of windows to CGWork

BOOL CCGWorkView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	// An CGWork window must be created with the following
	// flags and must NOT include CS_PARENTDC for the
	// class style.

	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}



int CCGWorkView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	InitializeCGWork();


	m_bShowBoundingBox = m_bShowFaceNormals = m_bShowVerNormals = false;
	m_bshowGivenFNormal = m_bshowGivenVNormal = true;//beccause they look cooler
	m_nActiveMesh = -1;//all are active
	return 0;
}


// This method initialized the CGWork system.
BOOL CCGWorkView::InitializeCGWork()
{
	m_pDC = new CClientDC(this);
	
	if ( NULL == m_pDC ) { // failure to get DC
		::AfxMessageBox(CString("Couldn't get a valid DC."));
		return FALSE;
	}

	CRect r;
	GetClientRect(&r);
	m_pDbDC = new CDC();
	m_pDbDC->CreateCompatibleDC(m_pDC);
	SetTimer(1, 1, NULL);
	m_pDbBitMap = CreateCompatibleBitmap(m_pDC->m_hDC, r.right, r.bottom);	
	m_pDbDC->SelectObject(m_pDbBitMap);
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView message handlers


void CCGWorkView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	if ( 0 >= cx || 0 >= cy ) {
		return;
	}

	// save the width and height of the current window
	m_WindowWidth = cx;
	m_WindowHeight = cy;

	// compute the aspect ratio
	// this will keep all dimension scales equal
	m_AspectRatio = (GLdouble)m_WindowWidth/(GLdouble)m_WindowHeight;

	CRect r;
	GetClientRect(&r);
	DeleteObject(m_pDbBitMap);
	m_pDbBitMap = CreateCompatibleBitmap(m_pDC->m_hDC, r.right, r.bottom);	
	m_pDbDC->SelectObject(m_pDbBitMap);
}


BOOL CCGWorkView::SetupViewingFrustum(void)
{
    return TRUE;
}


// This viewing projection gives us a constant aspect ration. This is done by
// increasing the corresponding size of the ortho cube.
BOOL CCGWorkView::SetupViewingOrthoConstAspect(void)
{
	return TRUE;
}





BOOL CCGWorkView::OnEraseBkgnd(CDC* pDC) 
{
	// Windows will clear the window with the background color every time your window 
	// is redrawn, and then CGWork will clear the viewport with its own background color.

	
	return true;
}





/////////////////////////////////////////////////////////////////////////////
// CCGWorkView drawing
/////////////////////////////////////////////////////////////////////////////

void CCGWorkView::OnDraw(CDC* pDC)
{

	CCGWorkDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
	    return;

	CRect r;

	GetClientRect(&r);
	CDC *pDCToUse = /*m_pDC*/m_pDbDC;
	auto w = r.Width();
	auto h = r.Height();


	HBITMAP bm = CreateCompatibleBitmap(*pDCToUse, w, h);
	SelectObject(*pDCToUse, bm);
	BITMAPINFO bminfo;
	bminfo.bmiHeader.biSize = sizeof(bminfo.bmiHeader);
	bminfo.bmiHeader.biWidth = w;
	bminfo.bmiHeader.biHeight = h;
	bminfo.bmiHeader.biPlanes = 1;
	bminfo.bmiHeader.biBitCount = 32;
	bminfo.bmiHeader.biCompression = BI_RGB;
	bminfo.bmiHeader.biSizeImage = 0;
	bminfo.bmiHeader.biXPelsPerMeter = 1;
	bminfo.bmiHeader.biYPelsPerMeter = 1;
	bminfo.bmiHeader.biClrUsed = 0;
	bminfo.bmiHeader.biClrImportant = 0;
	int* bits = new int[h * w];
	
	//notice tthat colors are stored in bits in an BGR order instead of RGB, so we replace it here
	COLORREF bgcolor = RGBToBGR(scene.getBackgroundColor());
	
	for (int i = 0; i < h*w; ++i)
		bits[i] = bgcolor;
	
	
	scene.draw(bits, w, h, m_bShowFaceNormals, m_bShowVerNormals, m_bshowGivenFNormal, m_bshowGivenVNormal, m_bShowBoundingBox);
	SetDIBits(*pDCToUse, bm, 0, h, bits, &bminfo, 0);
	m_pDC->BitBlt(r.left, r.top, r.Width(), r.Height(), pDCToUse, r.left, r.top, SRCCOPY);


	DeleteObject(bm);
	delete bits;



	
	/////////////////////////
	/*

	int margin;
	int x;
	int y;
	int ret;
	COLORREF color;
	CRect rect; // Client rectangle
	


	color = RGB(0, 0, 100);
	margin = 10;

	GetClientRect(&rect); //
	h = rect.bottom - rect.top;
	w = rect.right - rect.left;

	CPaintDC hdc(this); // device context for painting



	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP bm = CreateCompatibleBitmap(hdc, w, h);
	
	SelectObject(hdcMem, bm);
	BITMAPINFO bminfo;
	
	bminfo.bmiHeader.biSize = sizeof(bminfo.bmiHeader);
	bminfo.bmiHeader.biWidth = w;
	bminfo.bmiHeader.biHeight = h;
	bminfo.bmiHeader.biPlanes = 1;
	bminfo.bmiHeader.biBitCount = 32;
	bminfo.bmiHeader.biCompression = BI_RGB;
	bminfo.bmiHeader.biSizeImage = 0;
	bminfo.bmiHeader.biXPelsPerMeter = 1;
	bminfo.bmiHeader.biYPelsPerMeter = 1;
	bminfo.bmiHeader.biClrUsed = 0;
	bminfo.bmiHeader.biClrImportant = 0;

	int* bits = new int[h * w];

	for (x = rect.left + margin; x < rect.right - margin; x++)
	{
		for (y = rect.top + margin; y < rect.bottom - margin; y++)
		{
			bits[(x - rect.left) + (w * (y - rect.top))] = color;
		}
	}


	SetDIBits(hdcMem, bm, 0, h, bits, &bminfo, 0);

	ret = BitBlt(hdc, rect.left, rect.top, rect.right, rect.bottom, hdcMem, rect.left, rect.top, SRCCOPY);

	DeleteDC(hdcMem);
	DeleteObject(bm);
	delete bits;
	*/
}






/////////////////////////////////////////////////////////////////////////////
// CCGWorkView CGWork Finishing and clearing...

void CCGWorkView::OnDestroy() 
{
	CView::OnDestroy();

	// delete the DC
	if ( m_pDC ) {
		delete m_pDC;
	}

	if (m_pDbDC) {
		delete m_pDbDC;
	}
}



/////////////////////////////////////////////////////////////////////////////
// User Defined Functions

void CCGWorkView::RenderScene() {
	// do nothing. This is supposed to be overriden...

	return;
}


void CCGWorkView::OnFileLoad() 
{
	TCHAR szFilters[] = _T ("IRIT Data Files (*.itd)|*.itd|All Files (*.*)|*.*||");

	CFileDialog dlg(TRUE, _T("itd"), _T("*.itd"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,szFilters);

	if (dlg.DoModal() == IDOK) {
		m_strItdFileName = dlg.GetPathName();		// Full path and filename
		PngWrapper p;
		CGSkelProcessIritDataFiles(m_strItdFileName, 1);
		// Open the file and read it.
		// Your code here...

		
		scene.setInitialized(false);

		Invalidate();	// force a WM_PAINT for drawing.
	} 

}





// VIEW HANDLERS ///////////////////////////////////////////

// Note: that all the following Message Handlers act in a similar way.
// Each control or command has two functions associated with it.

void CCGWorkView::OnViewOrthographic() 
{
	m_nView = ID_VIEW_ORTHOGRAPHIC;
	m_bIsPerspective = false;
	Tmatd projection = TransformationMatrix<double>::ortho(-10.0, 10.0, -5.0, 5.0, -5.0, 5.0);
	scene.setProjection(projection);
	Invalidate();		// redraw using the new view.
}

void CCGWorkView::OnUpdateViewOrthographic(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nView == ID_VIEW_ORTHOGRAPHIC);
}

void CCGWorkView::OnViewPerspective() 
{
	m_nView = ID_VIEW_PERSPECTIVE;
	m_bIsPerspective = true;
	Tmatd projection = TransformationMatrix<double>::perspective(Scene::FOV, m_AspectRatio, 
		Scene::NEAR_PLANE, Scene::FAR_PLANE);
	scene.setProjection(projection);
	Invalidate();
}

void CCGWorkView::OnUpdateViewPerspective(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nView == ID_VIEW_PERSPECTIVE);
}




// ACTION HANDLERS ///////////////////////////////////////////

void CCGWorkView::OnActionRotate() 
{
	m_nAction = ID_ACTION_ROTATE;
}

void CCGWorkView::OnUpdateActionRotate(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_ROTATE);
}

void CCGWorkView::OnActionTranslate() 
{
	m_nAction = ID_ACTION_TRANSLATE;
}

void CCGWorkView::OnUpdateActionTranslate(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_TRANSLATE);
}

void CCGWorkView::OnActionScale() 
{
	m_nAction = ID_ACTION_SCALE;
}

void CCGWorkView::OnUpdateActionScale(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_SCALE);
}




// AXIS HANDLERS ///////////////////////////////////////////


// Gets calles when the X button is pressed or when the Axis->X menu is selected.
// The only thing we do here is set the ChildView member variable m_nAxis to the 
// selected axis.
void CCGWorkView::OnAxisX() 
{
	m_nAxis = ID_AXIS_X;
}

// Gets called when windows has to repaint either the X button or the Axis pop up menu.
// The control is responsible for its redrawing.
// It sets itself disabled when the action is a Scale action.
// It sets itself Checked if the current axis is the X axis.
void CCGWorkView::OnUpdateAxisX(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_X);
}


void CCGWorkView::OnAxisY() 
{
	m_nAxis = ID_AXIS_Y;
}

void CCGWorkView::OnUpdateAxisY(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_Y);
}


void CCGWorkView::OnAxisZ() 
{
	m_nAxis = ID_AXIS_Z;
}

void CCGWorkView::OnUpdateAxisZ(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_Z);
}


void CCGWorkView::OnFaceNormals()
{
	m_bShowFaceNormals = !m_bShowFaceNormals;
}

void CCGWorkView::OnUpdateFaceNormals(CCmdUI * pCmdUI)
{

	pCmdUI->SetCheck(m_bShowFaceNormals == true);
}

void CCGWorkView::OnVerNormals()
{
	m_bShowVerNormals = !m_bShowVerNormals;
}

void CCGWorkView::OnUpdateVerNormals(CCmdUI * pCmdUI)
{
	pCmdUI->SetCheck(m_bShowVerNormals == true);
}


//handles choosing what type of normals(faces and vertecis) should we show (doesn't handle if we show or not
void CCGWorkView::OnFaceNormalsCalc()
{
	m_bshowGivenFNormal = !m_bshowGivenFNormal;
}

void CCGWorkView::OnUpdateFaceNormalsCalc(CCmdUI * pCmdUI)
{
	pCmdUI->SetCheck(m_bshowGivenFNormal == true);
}


void CCGWorkView::OnVerNormalsCalc()
{
	m_bshowGivenVNormal = !m_bshowGivenVNormal;
}

void CCGWorkView::OnUpdateVerNormalsCalc(CCmdUI * pCmdUI)
{
	pCmdUI->SetCheck(m_bshowGivenVNormal == true);
}

//m_bShowBoundingBox

void CCGWorkView::OnShowBoundingBox()
{
	m_bShowBoundingBox = !m_bShowBoundingBox;
}

void CCGWorkView::OnUpdateShowBoundingBox(CCmdUI * pCmdUI)
{
	pCmdUI->SetCheck(m_bShowBoundingBox == true);
}


// OPTIONS HANDLERS ///////////////////////////////////////////




// LIGHT SHADING HANDLERS ///////////////////////////////////////////

void CCGWorkView::OnLightShadingFlat()
{
	m_nLightShading = ID_LIGHT_SHADING_FLAT;
}

void CCGWorkView::OnUpdateLightShadingFlat(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nLightShading == ID_LIGHT_SHADING_FLAT);
}


void CCGWorkView::OnLightShadingGouraud()
{
	m_nLightShading = ID_LIGHT_SHADING_GOURAUD;
}

void CCGWorkView::OnUpdateLightShadingGouraud(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nLightShading == ID_LIGHT_SHADING_GOURAUD);
}

// LIGHT SETUP HANDLER ///////////////////////////////////////////

void CCGWorkView::OnLightConstants()
{
	CLightDialog dlg;

	for (int id = LIGHT_ID_1; id < MAX_LIGHT; id++)
	{
		dlg.SetDialogData((LightID)id, m_lights[id]);
	}
	dlg.SetDialogData(LIGHT_ID_AMBIENT, m_ambientLight);

	if (dlg.DoModal() == IDOK)
	{
		for (int id = LIGHT_ID_1; id < MAX_LIGHT; id++)
		{
			m_lights[id] = dlg.GetDialogData((LightID)id);
		}
		m_ambientLight = dlg.GetDialogData(LIGHT_ID_AMBIENT);
	}
	Invalidate();
}

void CCGWorkView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	CView::OnTimer(nIDEvent);
	if (nIDEvent == 1)
		Invalidate();
}

void CCGWorkView::rotate(const int &angle)
{
	if (_curr_coordinate_system == CoordinateSystem::VIEW)
	{
		rotateView(angle * m_RotationSensitivity);
	}
	else if (_curr_coordinate_system == CoordinateSystem::MODEL)
	{
		rotateModel(angle * m_RotationSensitivity);
	}
}

void CCGWorkView::translate(const int &dist)
{
	if (_curr_coordinate_system == CoordinateSystem::VIEW)
	{
		translateView(dist * m_TranslationSensitivity);
	}
	else if(_curr_coordinate_system == CoordinateSystem::MODEL)
	{
		translateModel(dist * m_TranslationSensitivity);
	}
}

void CCGWorkView::scale(const int &scaling)
{
	if (scaling == 0)
	{
		return;
	}

	double factor = 1.1f * m_ScaleSensitivity; 
	if (scaling < 0)
	{
		factor = 1 / factor;
	}

	if (_curr_coordinate_system == CoordinateSystem::VIEW)
	{
		scaleView(factor);
	}
	else if (_curr_coordinate_system == CoordinateSystem::MODEL)
	{
		scaleModel(factor);
	}
}

void CCGWorkView::rotateModel(const double &val)
{
	auto& objects = scene.getObjects();
	if (m_nAxis == ID_AXIS_X)
	{
		if (m_nActiveMesh == -1)//do it for all meshs
			for (size_t i = 0; i < objects.size(); i++)
			{
				objects[i].rotateX(val);
			}

		else {
			objects[m_nActiveMesh].rotateX(val);
		}

	}
	else if (m_nAxis == ID_AXIS_Y)
	{
		if (m_nActiveMesh == -1)//do it for all meshs
			for (size_t i = 0; i < objects.size(); i++)
			{
				objects[i].rotateY(val);
			}

		else {
			objects[m_nActiveMesh].rotateY(val);
		}
	}
	else if (m_nAxis == ID_AXIS_Z)
	{
		if (m_nActiveMesh == -1)//do it for all meshs
			for (size_t i = 0; i < objects.size(); i++)
			{
				objects[i].rotateZ(val);
			}

		else {
			objects[m_nActiveMesh].rotateZ(val);
		}

	}
}

	void CCGWorkView::translateModel(const double &val) {


	auto& objects = scene.getObjects();

	if (m_nAxis == ID_AXIS_X)
	{
		if (m_nActiveMesh == -1)//do it for all meshs
			for (size_t i = 0; i < objects.size(); i++)
			{
				objects[i].translate(Vec3d(val, 0.0, 0.0));
			}

		else {
			objects[m_nActiveMesh].translate(Vec3d(val, 0.0, 0.0));
		}
	}
	else if (m_nAxis == ID_AXIS_Y)
	{

		if (m_nActiveMesh == -1)//do it for all meshs
			for (size_t i = 0; i < objects.size(); i++)
			{
				objects[i].translate(Vec3d(0.0, val, 0.0));
			}

		else {
			objects[m_nActiveMesh].translate(Vec3d(0.0, val, 0.0));
		}
	}
	else if (m_nAxis == ID_AXIS_Z)
	{
		if (m_nActiveMesh == -1)//do it for all meshs
			for (size_t i = 0; i < objects.size(); i++)
			{
				objects[i].translate(Vec3d(0.0, 0.0, val));
			}

		else {
			objects[m_nActiveMesh].translate(Vec3d(0.0, 0.0, val));
		}
	}

}

void CCGWorkView::scaleModel(const double &val)
{
	if (m_nAxis == ID_AXIS_X)
	{
		scene.getObjects()[0].scale(Vec3d(val, 1.0, 1.0));
	}
	else if (m_nAxis == ID_AXIS_Y)
	{
		scene.getObjects()[0].scale(Vec3d(1.0, val, 1.0));
	}
	else if (m_nAxis == ID_AXIS_Z)
	{
		scene.getObjects()[0].scale(Vec3d(1.0, 1.0, val));
	}
}

void CCGWorkView::rotateView(const double &val)
{
	if (m_nAxis == ID_AXIS_X)
	{
		scene.getView().rotateX(val);
	}
	else if (m_nAxis == ID_AXIS_Y)
	{
		scene.getView().rotateY(val);
	}
	else if (m_nAxis == ID_AXIS_Z)
	{
		scene.getView().rotateZ(val);
	}
}

void CCGWorkView::translateView(const double &val)
{
	if (m_nAxis == ID_AXIS_X)
	{
		scene.getView().translate(Vec3d(val, 0.0, 0.0));
	}
	else if (m_nAxis == ID_AXIS_Y)
	{
		scene.getView().translate(Vec3d(0.0, val, 0.0));
	}
	else if (m_nAxis == ID_AXIS_Z)
	{
		scene.getView().translate(Vec3d(0.0, 0.0, val));
	}
}

void CCGWorkView::scaleView(const double &val)
{
	if (m_nAxis == ID_AXIS_X)
	{
		scene.getView().scale(Vec3d(val, 1.0, 1.0));
	}
	else if (m_nAxis == ID_AXIS_Y)
	{
		scene.getView().scale(Vec3d(1.0, val, 1.0));
	}
	else if (m_nAxis == ID_AXIS_Z)
	{
		scene.getView().scale(Vec3d(1.0, 1.0, val));
	}
}

void CCGWorkView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!scene.isInitialized())
	{
		return;
	}

	static uint last_x = 0;
	uint curr_x = point.x;

	int dx = static_cast<int>(curr_x - last_x);

	if (nFlags == MK_LBUTTON)
	{
		if (m_nAction == ID_ACTION_ROTATE)
		{
			rotate(dx);
		}
		else if (m_nAction == ID_ACTION_TRANSLATE)
		{
			translate(dx);
		}
		else if (m_nAction == ID_ACTION_SCALE)
		{
			scale(dx);
		}
	}

	last_x = curr_x;

	CView::OnMouseMove(nFlags, point);
}


void CCGWorkView::OnCoordinatesystemView()
{
	_curr_coordinate_system = CoordinateSystem::VIEW;
}


void CCGWorkView::OnUpdateCoordinatesystemView(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(_curr_coordinate_system == CoordinateSystem::VIEW);
}


void CCGWorkView::OnCoordinatesystemModel()
{
	_curr_coordinate_system = CoordinateSystem::MODEL;
}


void CCGWorkView::OnUpdateCoordinatesystemModel(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(_curr_coordinate_system == CoordinateSystem::MODEL);
}







void CCGWorkView::OnColorsWireFrame()
{
	CColorDialog color_dialog;
	if (color_dialog.DoModal() == IDOK)
	{
		COLORREF color = color_dialog.GetColor();
		scene.setWireFrameColor(color, m_nActiveMesh);
	}
	RedrawWindow();
}

void CCGWorkView::OnColorsBackground()
{
	CColorDialog color_dialog;
	if (color_dialog.DoModal() == IDOK)
	{
		COLORREF color = color_dialog.GetColor();
		scene.setBackgroundColor(color);
	}
	RedrawWindow();
}


void CCGWorkView::OnColorsFaceNormals()
{
	CColorDialog color_dialog;
	if (color_dialog.DoModal() == IDOK)
	{
		COLORREF color = color_dialog.GetColor();
		scene.setFaceNormalsColor(color, m_nActiveMesh);
	}
	RedrawWindow();
}


void CCGWorkView::OnColorsVerticesNormals()
{
	CColorDialog color_dialog;
	if (color_dialog.DoModal() == IDOK)
	{
		COLORREF color = color_dialog.GetColor();
		scene.setVerticesNormalsColor(color, m_nActiveMesh);
	}
	RedrawWindow();
}

void CCGWorkView::OnColorsBoundingBox()
{
	CColorDialog color_dialog;
	if (color_dialog.DoModal() == IDOK)
	{
		COLORREF color = color_dialog.GetColor();
		scene.setBoundingBoxColor(color, m_nActiveMesh);
	}
	RedrawWindow();
}



void CCGWorkView::OnOptionsMousesensitivity()
{
	if (_mouse_sensetivity_dialog.DoModal() == IDOK)
	{
		m_RotationSensitivity = _mouse_sensetivity_dialog.getRotationSensetivity();
		m_TranslationSensitivity = _mouse_sensetivity_dialog.getTranslationSensetivity();
		m_ScaleSensitivity = _mouse_sensetivity_dialog.getScaleSensetivity();
	}
}


void CCGWorkView::OnOptionsPerspectivecontrol()
{
	if (_perspective_dialog.DoModal() == IDOK)
	{
		double fov_y_deg = _perspective_dialog.getFovYDeg();
		double near_plane = _perspective_dialog.getNear();
		double far_plane = _perspective_dialog.getFar();

		Tmatd projection = TransformationMatrix<double>::perspective(fov_y_deg, m_AspectRatio,
			near_plane, far_plane);
		scene.setProjection(projection);

		RedrawWindow();
	}
}

extern IPFreeformConvStateStruct CGSkelFFCState;
void CCGWorkView::OnOptionsPolygonfinenesstolerance()
{
	if (_polygon_dialog.DoModal() == IDOK)
	{
		CGSkelFFCState.FineNess = _polygon_dialog.getTolerance();
		if (CGSkelFFCState.FineNess < 2)
			CGSkelFFCState.FineNess = 2;//2 is the minimum
	}
}

void CCGWorkView::OnOptionsActiveMesh()
{

	CAtlString str = _T("available options for active mesh:\n(Mesh Name) : (id)\n");


	str.Append(_T("all : -1 \n"));
	auto objects = scene.getObjects();
	for (int i = 0; i < objects.size(); ++i) {

		CString cur;
		cur.FormatMessage(_T("%1% : %2!d! \n"), objects[i].getName(), i);
		str.Append( cur);
	}

	AfxMessageBox(str,MB_OK);


	if (_active_mesh_dialog.DoModal() == IDOK)
	{
		m_nActiveMesh = _active_mesh_dialog.getActive();
		if (m_nActiveMesh < 0 || m_nActiveMesh >= objects.size())
			m_nActiveMesh = -1;
	}
	_active_mesh_dialog.setActive(m_nActiveMesh);
}
