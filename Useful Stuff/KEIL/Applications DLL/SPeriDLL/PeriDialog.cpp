// PeriDialog.cpp : implementation file
// Peripheral Dialog: A/D Converter for ADuC812

#include "stdafx.h"
#include "Agsi.h"
#include "SPeriDLL.h"
#include "PeriDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Protoyptes for forward references
static void PeriUpdate (void);
static void PeriKill   (AGSIDLGD *pM);
static void PeriDisp   (AGSIMENU *pM);

/////////////////////////////////////////////////////////////////////////////
// CPeriDialog dialog
static CPeriDialog  * pCPeriDialog;
BYTE initflag = 0;

// must not use 'const' here !
//                     iOpen  Hwnd   Dlg Proc.  Rect: -1 := default    Update       Kill
AGSIDLGD PeriDlg =  {    0,   NULL,      NULL,  { -1, -1, -1, -1, },  PeriUpdate,  PeriKill };

//                    nDelim   *szText            *fp      nID   nDlgId         *pDlg;
AGSIMENU PeriMenu =  {  1,   "&A/D Converter"  , PeriDisp,  0,  IDD_ADCON,     &PeriDlg   };  // Peripheral Dialog


static DWORD  adccon1,  adccon2,  adccon3,  adcdatal,  adcdatah,  dmal,  dmah,  dmap;   // Current values
static DWORD  adccon1p, adccon2p, adccon3p, adcdatalp, adcdatahp, dmalp, dmahp, dmapp;  // Previous values


// Output 2-digit Hexnumber to Text-Control of given Dialog
void StringHex2 (CWnd * pCWnd, DWORD val)  {
  char locbuffer[20];
  sprintf (locbuffer, "0x%02X", val);
  pCWnd->SetWindowText (locbuffer);
}

// Output 4-digit Hexnumber to Text-Control of given Dialog
void StringHex4 (CWnd * pCWnd, DWORD val)  {
  char locbuffer[20];
  sprintf (locbuffer, "0x%04X", val);
  pCWnd->SetWindowText (locbuffer);
}

// Output 6-digit Hexnumber to Text-Control of given Dialog
void StringHex6 (CWnd * pCWnd, DWORD val)  {
  char locbuffer[20];
  sprintf (locbuffer, "0x%06X", val);
  pCWnd->SetWindowText (locbuffer);
}

// Output Float number to Text-Control of given Dialog
void StringFloat (CWnd * pCWnd, float val)  {
  char locbuffer[20];
  sprintf (locbuffer, "%1.4f", val);
  pCWnd->SetWindowText (locbuffer);
}

static const char INPUT_ERR_TITLE [] = "Invalid number";
static const char INPUT_ERRMSG[]     = "You have entered an invalid number!\n"
                                       "The previous value will be restored.\n"
                                       "Examples: 0x12\n";

static const char INPUT_OVR_TITLE [] = "Out of range";
static const char INPUT_OVRMSG[]     = "You have entered a number that is out of range!\n";

long GetDlg8BNumber (CWnd * pCWnd, DWORD oldval) {
  DWORD temp;
  WORD n;
  char lbuf[100];
  
  n = pCWnd->GetWindowText (lbuf, 100);
  lbuf[n] = '\0';  /* terminate string */
  n = sscanf(lbuf, "%x", &temp);
  if (n != 1) {
    MessageBeep(MB_ICONEXCLAMATION);
    pCWnd->MessageBox(&INPUT_ERRMSG[0], &INPUT_ERR_TITLE[0], MB_OK|MB_ICONSTOP); 
    StringHex2 (pCWnd, oldval);
    return(-1);
  }
  if (temp > 0xFF) {
    MessageBeep(MB_ICONEXCLAMATION);
    pCWnd->MessageBox(INPUT_OVRMSG, INPUT_OVR_TITLE, MB_OK | MB_ICONSTOP); 
    StringHex2 (pCWnd, oldval);
    return(-1);
  }
  StringHex2 (pCWnd, temp);
  return(temp);
}

long GetDlg16BNumber (CWnd *pCWnd, DWORD oldval) {
  DWORD temp;
  WORD n;
  char lbuf[100];
  
  n = pCWnd->GetWindowText (lbuf, 100);
  lbuf[n] = '\0';  /* terminate string */
  n = sscanf(lbuf, "%x", &temp);
  if (n != 1) {
    MessageBeep(MB_ICONEXCLAMATION);
    pCWnd->MessageBox(&INPUT_ERRMSG[0],&INPUT_ERR_TITLE[0],MB_OK|MB_ICONSTOP); 
    StringHex4 (pCWnd, oldval);
    return(-1);
  }
  if (temp > 0xFFFF) {
    MessageBeep(MB_ICONEXCLAMATION);
    pCWnd->MessageBox(INPUT_OVRMSG, INPUT_OVR_TITLE, MB_OK | MB_ICONSTOP); 
    StringHex4 (pCWnd, oldval);
    return(-1);
  }
  StringHex4 (pCWnd, temp);
  return(temp);
}

long GetDlg24BNumber (CWnd *pCWnd, DWORD oldval) {
  DWORD temp;
  WORD n;
  char lbuf[100];
  
  n = pCWnd->GetWindowText (lbuf, 100);
  lbuf[n] = '\0';  /* terminate string */
  n = sscanf(lbuf, "%x", &temp);
  if (n != 1) {
    MessageBeep(MB_ICONEXCLAMATION);
    pCWnd->MessageBox(INPUT_ERRMSG, INPUT_ERR_TITLE, MB_OK | MB_ICONSTOP); 
    StringHex6 (pCWnd, oldval);
    return(-1);
  }
  if (temp > 0xFFFFFF) {
    MessageBeep(MB_ICONEXCLAMATION);
    pCWnd->MessageBox(INPUT_OVRMSG, INPUT_OVR_TITLE, MB_OK | MB_ICONSTOP); 
    StringHex6 (pCWnd, oldval);
    return(-1);
  }
  StringHex6 (pCWnd, temp);
  return(temp);
}


static const char INPUT_F_ERR_TITLE [] = "Invalid float number";
static const char INPUT_F_ERRMSG[]     = "You have entered an invalid float number!\n"
                                         "The previous value will be restored.\n"
                                         "Example: 1.234\n";

float GetDlgFloat (CWnd * pCWnd, float oldval) {
  WORD n;
  float f;
  char lbuf[100];
  
  n = pCWnd->GetWindowText (lbuf, 100);
  lbuf[n] = '\0';  /* terminate string */
  n = sscanf(lbuf, "%f", &f);
  if (n != 1) {
    MessageBeep(MB_ICONEXCLAMATION);
    pCWnd->MessageBox(INPUT_F_ERRMSG, INPUT_F_ERR_TITLE, MB_OK | MB_ICONSTOP); 
    sprintf (lbuf, "%1.4f", oldval);
    pCWnd->SetWindowText (lbuf);
    return((float)-1.9876e-36);
  }
  sprintf (lbuf, "%1.4f", f);
  pCWnd->SetWindowText (lbuf);
  return(f);
}


// This function handles a byte input in a dialog
void HandleByteInput(CWnd * pCWnd, AGSIADDR sfr) {
  DWORD oldval, oldvalp, tmp;

  Agsi.ReadSFR(sfr, &oldval, &oldvalp, 0xFF);
  tmp = GetDlg8BNumber (pCWnd, oldval);
  if (tmp != -1) {
    Agsi.WriteSFR(sfr, tmp, 0xFF);
	Agsi.UpdateWindows();
  }
}

// This function handles a word input in a dialog
void HandleWordInput(CWnd * pCWnd, AGSIADDR sfrl, AGSIADDR sfrh) {
  DWORD oldval, tmp, tmpp;

  Agsi.ReadSFR(sfrl, &tmp, &tmpp, 0xFF);
  oldval = tmp;
  Agsi.ReadSFR(sfrh, &tmp, &tmpp, 0xFF);
  oldval += tmp << 8;
  tmp = GetDlg16BNumber (pCWnd, oldval);
  if (tmp != -1) {
    Agsi.WriteSFR(sfrl, tmp, 0xFF);
    Agsi.WriteSFR(sfrh, tmp >> 8, 0xFF);
	Agsi.UpdateWindows();
  }
}

// This function handles a 3 byte input in a dialog
void Handle3ByteInput(CWnd * pCWnd, AGSIADDR sfrl, AGSIADDR sfrh, AGSIADDR sfrp) {
  DWORD oldval, tmp, tmpp;

  Agsi.ReadSFR(sfrl, &tmp, &tmpp, 0xFF);
  oldval = tmp;
  Agsi.ReadSFR(sfrh, &tmp, &tmpp, 0xFF);
  oldval += tmp << 8;
  Agsi.ReadSFR(sfrp, &tmp, &tmpp, 0xFF);
  oldval += tmp << 16;
  tmp = GetDlg24BNumber (pCWnd, oldval);
  if (tmp != -1) {
    Agsi.WriteSFR(sfrl, tmp, 0xFF);
    Agsi.WriteSFR(sfrh, tmp >> 8, 0xFF);
    Agsi.WriteSFR(sfrp, tmp >> 16, 0xFF);
	Agsi.UpdateWindows();
  }
}

// This function handles a float number input in a dialog
void HandleFloatInput(CWnd * pCWnd, AGSIVTR vtr) {
  union fv tmp;
  float result;

  Agsi.ReadVTR(vtr, &tmp.DW);
  result = GetDlgFloat (pCWnd, tmp.f);
  if (result != -1.9876e-36) {
	tmp.f = result;
    Agsi.WriteVTR(vtr, tmp.DW);
  }
}

// This function is intended to be called when a checkbox is clicked (SFR bit)
void WriteBit(AGSIADDR sfr, DWORD mask, DWORD set) {
  DWORD value;

  value = (set == 0) ? 0 : 0xFF;
  Agsi.WriteSFR(sfr, value, mask);
  Agsi.UpdateWindows();
}

// This function is intended to be called when a checkbox is clicked (VTR bit)
void WriteBitVTR(AGSIVTR vtr, DWORD mask, DWORD set) {
  DWORD value;	

  Agsi.ReadVTR(vtr, &value);
  if (set) value |= mask;
  else value &= ~mask;
  Agsi.WriteVTR(vtr, value);
  Agsi.UpdateWindows();
}


static void PeriUpdate (void)  {            // Update Function
  if (pCPeriDialog) pCPeriDialog->Update();
}

static void PeriKill (AGSIDLGD *pM)  {      // Kill Function
  if (pCPeriDialog == NULL) return;
  pCPeriDialog->SendMessage (WM_CLOSE);
  pCPeriDialog  = NULL;
  pM->iOpen = 0;
  pM->hw    = NULL;
}

static void PeriDisp (AGSIMENU *pM)  {
  if (pM->pDlg->hw != NULL)  {              // created
    PeriKill (pM->pDlg);                    // close
  } else  {
//    AFX_MANAGE_STATE(AfxGetStaticModuleState());// -- not necessary.
    pCPeriDialog = new CPeriDialog (pM, NULL);     // modeless construction
    if (pCPeriDialog != NULL)  {                   // construction was Ok.
      pM->pDlg->hw = pCPeriDialog->m_hWnd;         // Dialog handle
    }
  }
}

CPeriDialog::CPeriDialog (AGSIMENU *pMen, CWnd *pWnd)  {
  pM = pMen;                     // save DYM-Descriptor locally.
  Create (IDD_ADCON, pWnd);
  pCPeriDialog = this;
}


// standard constructor does not work here because we are using modeless dialogs
//CPeriDialog::CPeriDialog(CWnd* pParent /*=NULL*/)
//	: CDialog(CPeriDialog::IDD, pParent)
//{
	//{{AFX_DATA_INIT(CPeriDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
//}


void CPeriDialog::Update (void)  {          /* Update Dialog Contents */
  DWORD  rate, convst, fosc;
  static DWORD  rated;
  static DWORD  convstd;
  union  fv     ain0,  ain1,  ain2,  ain3,  ain4,  ain5,  ain6,  ain7 , temp,  vref;
  static float  ain0d, ain1d, ain2d, ain3d, ain4d, ain5d, ain6d, ain7d, tempd, vrefd;
  static DWORD  adccon1d, adccon2d, adccon3d, adcdatald, adcdatahd, dmald, dmahd, dmapd;
  char locbuffer[20];

  Agsi.ReadSFR(ADCCON1,  &adccon1,  &adccon1p,  0xFF);
  Agsi.ReadSFR(ADCCON2,  &adccon2,  &adccon2p,  0xFF);
  Agsi.ReadSFR(ADCCON3,  &adccon3,  &adccon3p,  0xFF);
  Agsi.ReadSFR(ADCDATAL, &adcdatal, &adcdatalp, 0xFF);
  Agsi.ReadSFR(ADCDATAH, &adcdatah, &adcdatahp, 0xFF);
  Agsi.ReadSFR(DMAL,     &dmal,     &dmalp,     0xFF);
  Agsi.ReadSFR(DMAH,     &dmah,     &dmahp,     0xFF);
  Agsi.ReadSFR(DMAP,     &dmap,     &dmapp,     0xFF);

  Agsi.ReadVTR(AIN0,   &ain0.DW);
  Agsi.ReadVTR(AIN1,   &ain1.DW);
  Agsi.ReadVTR(AIN2,   &ain2.DW);
  Agsi.ReadVTR(AIN3,   &ain3.DW);
  Agsi.ReadVTR(AIN4,   &ain4.DW);
  Agsi.ReadVTR(AIN5,   &ain5.DW);
  Agsi.ReadVTR(AIN6,   &ain6.DW);
  Agsi.ReadVTR(AIN7,   &ain7.DW);
  Agsi.ReadVTR(TEMP,   &temp.DW);
  Agsi.ReadVTR(VREF,   &vref.DW);
  Agsi.ReadVTR(XTAL,   &fosc);
  Agsi.ReadVTR(CONVST, &convst);

  if (initflag == 1) {
    adccon1d  = ~adccon1;
	adccon2d  = ~adccon2;
	adccon3d  = ~adccon3;
    adcdatald = ~adcdatal;
    adcdatahd = ~adcdatah;
	dmald     = ~dmal;
	dmahd     = ~dmah;
	dmapd     = ~dmap;
    ain0d     = ain0.f + 1.0f;
    ain1d     = ain1.f + 1.0f;
    ain2d     = ain2.f + 1.0f;
    ain3d     = ain3.f + 1.0f;
    ain4d     = ain4.f + 1.0f;
    ain5d     = ain5.f + 1.0f;
    ain6d     = ain6.f + 1.0f;
    ain7d     = ain7.f + 1.0f;
    tempd     = temp.f + 1.0f;
    vrefd     = vref.f + 1.0f;
	rated     = 0xFFFFFFFF;
	convstd   = convst ^ 0x01;
  }

  if (initflag == 2) adccon3d = ~adccon3;

  initflag = 0;

  if (adccon1d != adccon1) {
    StringHex2 (GetDlgItem (IDC_AD_ADCCON1), adccon1);
    if ((adccon1d ^ adccon1) & 0xC0) ((CComboBox *) GetDlgItem (IDC_AD_MODE))->SetCurSel((adccon1 & 0xC0) >> 6);
    if ((adccon1d ^ adccon1) & 0x30) ((CComboBox *) GetDlgItem (IDC_AD_CLOCK))->SetCurSel((adccon1 & 0x30) >> 4);
    if ((adccon1d ^ adccon1) & 0x0C) ((CComboBox *) GetDlgItem (IDC_AD_ACQUISITION))->SetCurSel((adccon1 & 0x0C) >> 2);
    if ((adccon1d ^ adccon1) & 0x02) ((CButton *) GetDlgItem (IDC_AD_T2C))->SetCheck ((adccon1 & 0x02) ? 1 : 0);
    if ((adccon1d ^ adccon1) & 0x01) ((CButton *) GetDlgItem (IDC_AD_EXC))->SetCheck ((adccon1 & 0x01) ? 1 : 0);
	adccon1d = adccon1;
  }

  if (adccon2d != adccon2) {
    StringHex2 (GetDlgItem (IDC_AD_ADCCON2), adccon2);
    if ((adccon2d ^ adccon2) & 0x0F) ((CComboBox *) GetDlgItem (IDC_AD_CHANNEL))->SetCurSel(adccon2 & 0x0F);
    if ((adccon2d ^ adccon2) & 0x10) ((CButton *) GetDlgItem (IDC_AD_SCONV))->SetCheck ((adccon2 & 0x10) ? 1 : 0);
    if ((adccon2d ^ adccon2) & 0x20) ((CButton *) GetDlgItem (IDC_AD_CCONV))->SetCheck ((adccon2 & 0x20) ? 1 : 0);
    if ((adccon2d ^ adccon2) & 0x40) ((CButton *) GetDlgItem (IDC_AD_DMA))->SetCheck ((adccon2 & 0x40) ? 1 : 0);
    if ((adccon2d ^ adccon2) & 0x80) ((CButton *) GetDlgItem (IDC_AD_ADCI))->SetCheck ((adccon2 & 0x80) ? 1 : 0);
	adccon2d = adccon2;
  }

  if (adccon3d != adccon3) {
    StringHex2 (GetDlgItem (IDC_AD_ADCCON3), adccon3);
    if ((adccon3d ^ adccon3) & 0x80) ((CButton *) GetDlgItem (IDC_AD_BUSY))->SetCheck ((adccon3 & 0x80) ? 1 : 0);
	adccon3d = adccon3;
  }

  if ((adcdatald != adcdatal) || (adcdatahd != adcdatah)) {
    StringHex4 (GetDlgItem (IDC_AD_ADCDATA), (adcdatah << 8) + adcdatal);
	adcdatald = adcdatal;
	adcdatahd = adcdatah;
  }

  if ((dmald != dmal) || (dmahd != dmah) || (dmapd != dmap)) {
    StringHex6 (GetDlgItem (IDC_AD_DMAPHL), (dmap << 16) + (dmah << 8) + dmal);
    dmald = dmal;
    dmahd = dmah;
    dmapd = dmap;
  }

  rate = (fosc / (((adccon1 & 0x0C) >> 2) + 17)) >> ((adccon1 & 0x30) >> 4);
  if (rated != rate) {
    sprintf (locbuffer, "%ld", rate);
    SetDlgItemText (IDC_AD_RATE, locbuffer);
	rated = rate;
  }

  if ((convstd ^ convst) & 0x01) {
    ((CButton *) GetDlgItem (IDC_AD_CONVST))->SetCheck ((convst & 0x01) ? 1 : 0);
	convstd = convst;
  }

  if (ain0d != ain0.f) { StringFloat (GetDlgItem (IDC_AD_AIN0), ain0.f); ain0d = ain0.f; }
  if (ain1d != ain1.f) { StringFloat (GetDlgItem (IDC_AD_AIN1), ain1.f); ain1d = ain1.f; }
  if (ain2d != ain2.f) { StringFloat (GetDlgItem (IDC_AD_AIN2), ain2.f); ain2d = ain2.f; }
  if (ain3d != ain3.f) { StringFloat (GetDlgItem (IDC_AD_AIN3), ain3.f); ain3d = ain3.f; }
  if (ain4d != ain4.f) { StringFloat (GetDlgItem (IDC_AD_AIN4), ain4.f); ain4d = ain4.f; }
  if (ain5d != ain5.f) { StringFloat (GetDlgItem (IDC_AD_AIN5), ain5.f); ain5d = ain5.f; }
  if (ain6d != ain6.f) { StringFloat (GetDlgItem (IDC_AD_AIN6), ain6.f); ain6d = ain6.f; }
  if (ain7d != ain7.f) { StringFloat (GetDlgItem (IDC_AD_AIN7), ain7.f); ain7d = ain7.f; }
  if (vrefd != vref.f) { StringFloat (GetDlgItem (IDC_AD_VREF), vref.f); vrefd = vref.f; }
  if (tempd != temp.f) {
    sprintf (locbuffer, "%1.1f", temp.f);
    (GetDlgItem (IDC_AD_TEMP))->SetWindowText (locbuffer);
	tempd = temp.f;
  }

}


BEGIN_MESSAGE_MAP(CPeriDialog, CDialog)
	//{{AFX_MSG_MAP(CPeriDialog)
	ON_WM_ACTIVATE()
	ON_WM_CLOSE()
	ON_CBN_SELCHANGE(IDC_AD_MODE, OnSelchangeAdMode)
	ON_CBN_SELCHANGE(IDC_AD_CLOCK, OnSelchangeAdClock)
	ON_CBN_SELCHANGE(IDC_AD_ACQUISITION, OnSelchangeAdAcquisition)
	ON_CBN_SELCHANGE(IDC_AD_CHANNEL, OnSelchangeAdChannel)
	ON_EN_KILLFOCUS(IDC_AD_ADCCON1, OnKillfocusAdAdccon1)
	ON_EN_KILLFOCUS(IDC_AD_ADCCON2, OnKillfocusAdAdccon2)
	ON_EN_KILLFOCUS(IDC_AD_ADCCON3, OnKillfocusAdAdccon3)
	ON_EN_KILLFOCUS(IDC_AD_ADCDATA, OnKillfocusAdAdcdata)
	ON_EN_KILLFOCUS(IDC_AD_DMAPHL, OnKillfocusAdDmaphl)
	ON_BN_CLICKED(IDC_AD_CONVST, OnAdConvst)
	ON_BN_CLICKED(IDC_AD_SCONV, OnAdSconv)
	ON_BN_CLICKED(IDC_AD_CCONV, OnAdCconv)
	ON_BN_CLICKED(IDC_AD_DMA, OnAdDma)
	ON_BN_CLICKED(IDC_AD_EXC, OnAdExc)
	ON_BN_CLICKED(IDC_AD_T2C, OnAdT2c)
	ON_BN_CLICKED(IDC_AD_ADCI, OnAdAdci)
	ON_BN_CLICKED(IDC_AD_BUSY, OnAdBusy)
	ON_EN_KILLFOCUS(IDC_AD_AIN0, OnKillfocusAdAin0)
	ON_EN_KILLFOCUS(IDC_AD_AIN1, OnKillfocusAdAin1)
	ON_EN_KILLFOCUS(IDC_AD_AIN2, OnKillfocusAdAin2)
	ON_EN_KILLFOCUS(IDC_AD_AIN3, OnKillfocusAdAin3)
	ON_EN_KILLFOCUS(IDC_AD_AIN4, OnKillfocusAdAin4)
	ON_EN_KILLFOCUS(IDC_AD_AIN5, OnKillfocusAdAin5)
	ON_EN_KILLFOCUS(IDC_AD_AIN6, OnKillfocusAdAin6)
	ON_EN_KILLFOCUS(IDC_AD_AIN7, OnKillfocusAdAin7)
	ON_EN_KILLFOCUS(IDC_AD_TEMP, OnKillfocusAdTemp)
	ON_EN_KILLFOCUS(IDC_AD_VREF, OnKillfocusAdVref)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPeriDialog message handlers

void CPeriDialog::PostNcDestroy() {

  delete this;              // delete the new'ed object
  pCPeriDialog = NULL;      // clear external Object pointer here.
}

BOOL CPeriDialog::OnInitDialog() {
  CDialog::OnInitDialog();

  // TODO: Add extra initialization here

  // Restore Position (Only moving without resizing)
  if (PeriDlg.rc.left != -1) {
 	SetWindowPos(NULL,                        /* placement order - not used */
                 PeriDlg.rc.left,             /* left */
                 PeriDlg.rc.top,              /* top  */
                 0,                           /* width - not used */
                 0,                           /* height - not used */
                 SWP_NOSIZE | SWP_NOZORDER);  /* flags */
  }
	
  initflag =1;
  Update();
  return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPeriDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) {
  CDialog::OnActivate(nState, pWndOther, bMinimized);
  
  switch (nState)  {
    case WA_INACTIVE:
      Agsi.HandleFocus(NULL);      // Clear Modeless Handle
      break;
    case WA_ACTIVE:
    case WA_CLICKACTIVE:
      Agsi.HandleFocus(m_hWnd);    // Set Modeless Handle
      break;
  }
}

void CPeriDialog::OnClose() {

  GetWindowRect (&pM->pDlg->rc);  // save Window position
  pM->pDlg->hw = NULL;            // clear m_hWnd
  DestroyWindow();                //--- modeless
}


void CPeriDialog::OnSelchangeAdMode() {
  int i;
  DWORD val;
	
  i = ((CComboBox *) GetDlgItem (IDC_AD_MODE))->GetCurSel(); 
  val = (i & 3) << 6;
  Agsi.WriteSFR(ADCCON1, val, 0xC0);
  Agsi.UpdateWindows();
}

void CPeriDialog::OnSelchangeAdClock() {
  int i;
  DWORD val;
	
  i = ((CComboBox *) GetDlgItem (IDC_AD_CLOCK))->GetCurSel();
  val = (i & 3) << 4;
  Agsi.WriteSFR(ADCCON1, val, 0x30);
  Agsi.UpdateWindows();
}

void CPeriDialog::OnSelchangeAdAcquisition() {
  int i;
  DWORD val;
	
  i = ((CComboBox *) GetDlgItem (IDC_AD_ACQUISITION))->GetCurSel();
  val = (i & 3) << 2;
  Agsi.WriteSFR(ADCCON1, val, 0x0C);
  Agsi.UpdateWindows();
}

void CPeriDialog::OnSelchangeAdChannel() {
  int i;
  DWORD val;
	
  i = ((CComboBox *) GetDlgItem (IDC_AD_CHANNEL))->GetCurSel();
  val = i & 0x0F;
  Agsi.WriteSFR(ADCCON2, val, 0x0F);
  Agsi.UpdateWindows();
}


void CPeriDialog::OnKillfocusAdAdccon1() { HandleByteInput (GetDlgItem(IDC_AD_ADCCON1), ADCCON1); } 
void CPeriDialog::OnKillfocusAdAdccon2() { HandleByteInput (GetDlgItem(IDC_AD_ADCCON2), ADCCON2); } 
void CPeriDialog::OnKillfocusAdAdccon3() { HandleByteInput (GetDlgItem(IDC_AD_ADCCON3), ADCCON3); } 
void CPeriDialog::OnKillfocusAdAdcdata() { HandleWordInput (GetDlgItem(IDC_AD_ADCDATA), ADCDATAL, ADCDATAH); } 
void CPeriDialog::OnKillfocusAdDmaphl()  { Handle3ByteInput (GetDlgItem(IDC_AD_DMAPHL), DMAL, DMAH, DMAP); } 

void CPeriDialog::OnAdConvst() { WriteBitVTR(CONVST, 0x01, IsDlgButtonChecked (IDC_AD_CONVST)); } 

void CPeriDialog::OnAdSconv()  { WriteBit(ADCCON2, 0x10, IsDlgButtonChecked (IDC_AD_SCONV)); }
void CPeriDialog::OnAdCconv()  { WriteBit(ADCCON2, 0x20, IsDlgButtonChecked (IDC_AD_CCONV)); }
void CPeriDialog::OnAdDma()    { WriteBit(ADCCON2, 0x40, IsDlgButtonChecked (IDC_AD_DMA)); }
void CPeriDialog::OnAdExc()    { WriteBit(ADCCON1, 0x01, IsDlgButtonChecked (IDC_AD_EXC)); }
void CPeriDialog::OnAdT2c()    { WriteBit(ADCCON1, 0x02, IsDlgButtonChecked (IDC_AD_T2C)); }
void CPeriDialog::OnAdAdci()   { WriteBit(ADCCON2, 0x80, IsDlgButtonChecked (IDC_AD_ADCI)); }
void CPeriDialog::OnAdBusy()   { WriteBit(ADCCON3, 0x80, IsDlgButtonChecked (IDC_AD_BUSY)); }

void CPeriDialog::OnKillfocusAdAin0() { HandleFloatInput (GetDlgItem(IDC_AD_AIN0), AIN0); }
void CPeriDialog::OnKillfocusAdAin1() { HandleFloatInput (GetDlgItem(IDC_AD_AIN1), AIN1); }
void CPeriDialog::OnKillfocusAdAin2() { HandleFloatInput (GetDlgItem(IDC_AD_AIN2), AIN2); }
void CPeriDialog::OnKillfocusAdAin3() { HandleFloatInput (GetDlgItem(IDC_AD_AIN3), AIN3); }
void CPeriDialog::OnKillfocusAdAin4() { HandleFloatInput (GetDlgItem(IDC_AD_AIN4), AIN4); }
void CPeriDialog::OnKillfocusAdAin5() { HandleFloatInput (GetDlgItem(IDC_AD_AIN5), AIN5); }
void CPeriDialog::OnKillfocusAdAin6() { HandleFloatInput (GetDlgItem(IDC_AD_AIN6), AIN6); }
void CPeriDialog::OnKillfocusAdAin7() { HandleFloatInput (GetDlgItem(IDC_AD_AIN7), AIN7); }

void CPeriDialog::OnKillfocusAdTemp() { HandleFloatInput (GetDlgItem(IDC_AD_TEMP), TEMP); }
void CPeriDialog::OnKillfocusAdVref() { HandleFloatInput (GetDlgItem(IDC_AD_VREF), VREF); }


// The following two functions and the corresponding invisible buttons are necessary 
// to handle the behavior of the ESC and Enter Keys 
void CPeriDialog::OnOK() {
	
//	CDialog::OnOK();  // Do nothing when <Enter> is pressed
}

void CPeriDialog::OnCancel() {
	
  OnClose();            // Close Dialog when <ESC> is pressed 
}
