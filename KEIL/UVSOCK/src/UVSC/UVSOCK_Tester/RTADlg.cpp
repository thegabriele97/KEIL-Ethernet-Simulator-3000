// RTADlg.cpp : implementation file
//

#include "stdafx.h"
#include "RTADlg.h"

#include "..\Socket\UvSock.h"
#include "TraceEvent.h"


/* Real-Time Agent command definitions */
typedef enum __RTA_COMMANDS {  
  RTA_CMD_SYNC            = 0x00,

  /* Normal internal command codes */
  RTA_CMD_ERROR           = 0x01,
  RTA_CMD_TERM_DATA       = 0x02,
  RTA_CMD_SET_ADDRESS     = 0x03,
  RTA_CMD_MEM_WRITE_8     = 0x04,
  RTA_CMD_MEM_WRITE_16    = 0x05,
  RTA_CMD_MEM_WRITE_32    = 0x06,
  RTA_CMD_RTX_TASK_SWITCH = 0x07,
  RTA_CMD_RTX_MON_START   = 0x08,
  RTA_CMD_RTX_MON_STOP    = 0x09,
  RTA_CMD_RTX_TASK_CHANGE = 0x0A,
  RTA_CMD_RTX_CLOCK_INFO  = 0x0B,
  
  /* Synchronous response command codes (must have bit 7 set) */
  RTA_CMD_GET_VERSION     = 0x80,
  RTA_CMD_MEM_READ_8      = 0x81,
  RTA_CMD_MEM_READ_16     = 0x82,
  RTA_CMD_MEM_READ_32     = 0x83,
    
  /* Reserved for user custom applications */
  RTA_CMD_CUSTOM_0        = 0x70,
  RTA_CMD_CUSTOM_1        = 0x71,
  RTA_CMD_CUSTOM_2        = 0x72,
  RTA_CMD_CUSTOM_3        = 0x73,
  RTA_CMD_CUSTOM_4        = 0x74,
  RTA_CMD_CUSTOM_5        = 0x75,
  RTA_CMD_CUSTOM_6        = 0x76,
  RTA_CMD_CUSTOM_7        = 0x77,
  RTA_CMD_CUSTOM_8        = 0x78,
  RTA_CMD_CUSTOM_9        = 0x79,
  RTA_CMD_CUSTOM_10       = 0x7A,
  RTA_CMD_CUSTOM_11       = 0x7B,
  RTA_CMD_CUSTOM_12       = 0x7C,
  RTA_CMD_CUSTOM_13       = 0x7D,
  RTA_CMD_CUSTOM_14       = 0x7E,
  RTA_CMD_CUSTOM_15       = 0x7F,

  /* Speical Unknown command definition for reset */
  RTA_CMD_UNKNOWN         = 0x0F
} RTA_COMMANDS;



// Mode the RTAH is to be run in
typedef enum _tag_RTAH_MODE {
  RTAH_MODE_SIM,
  RTAH_MODE_TARGET
} RTAH_MODE;

// RTAH message header split
typedef struct _tag_msg_header_t {
  unsigned int length     : 8;    // 0x000000XX
  unsigned int seq_no     : 8;    // 0x0000XX00
  unsigned int type       : 8;    // 0x00XX0000
  unsigned int fixed      : 8;    // 0xXX000000
} msg_header_t;

// RTAH message header union
typedef union _tag_msg_header_u {
    msg_header_t split;
    unsigned int raw;
} msg_header_u;

// RTAH message structure
typedef struct _tag_rtah_msg_t {
  msg_header_u header;
  const unsigned int data[255];
} rtah_msg_t;


// CRTADlg dialog

IMPLEMENT_DYNAMIC(CRTADlg, CDialog)

CRTADlg::CRTADlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRTADlg::IDD, pParent)
{
  // Init 'static' RTA dialog
  this->Create(IDD_RTA, this);

  // Change the font of the dialogbox
  //RTATraceFont.CreatePointFont(80, _T("Courier New"));
  //m_listRxRTA.SetFont(&RTATraceFont);
}

CRTADlg::~CRTADlg()
{
}

void CRTADlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_SHOWRAW, m_CheckRaw);
  DDX_Control(pDX, IDC_CHECKCUST1, m_showCustom[0]);
  DDX_Control(pDX, IDC_CHECKCUST2, m_showCustom[1]);
  DDX_Control(pDX, IDC_CHECKCUST3, m_showCustom[2]);
  DDX_Control(pDX, IDC_CHECKCUST4, m_showCustom[3]);
  DDX_Control(pDX, IDC_CHECKCUST5, m_showCustom[4]);
  DDX_Control(pDX, IDC_CHECKCUST6, m_showCustom[5]);
  DDX_Control(pDX, IDC_CHECKCUST7, m_showCustom[6]);
  DDX_Control(pDX, IDC_CHECKCUST8, m_showCustom[7]);
  DDX_Control(pDX, IDC_CHECKCUST9, m_showCustom[8]);
  DDX_Control(pDX, IDC_CHECKCUST10, m_showCustom[9]);
  DDX_Control(pDX, IDC_CHECKCUST11, m_showCustom[10]);
  DDX_Control(pDX, IDC_CHECKCUST12, m_showCustom[11]);
  DDX_Control(pDX, IDC_CHECKCUST13, m_showCustom[12]);
  DDX_Control(pDX, IDC_CHECKCUST14, m_showCustom[13]);
  DDX_Control(pDX, IDC_CHECKCUST15, m_showCustom[14]);
  DDX_Control(pDX, IDC_CHECKCUST16, m_showCustom[15]);
  DDX_Control(pDX, IDC_CHECKTASKSWITCH, m_CheckTaskSwitch);
  DDX_Control(pDX, IDC_LISTRXRTA, m_listRxRTA);
}


BEGIN_MESSAGE_MAP(CRTADlg, CDialog)
  ON_BN_CLICKED(IDC_BUTTONCLEARWINDOW, &CRTADlg::OnBnClickedButtonclearwindow)
  ON_BN_CLICKED(IDC_CHECKCUST1, &CRTADlg::OnBnClickedCheckcust1)
END_MESSAGE_MAP()


// CRTADlg message handlers


BOOL CRTADlg::OnInitDialog()  {
  CDialog::OnInitDialog();

  // Set up the log box
  m_listRxRTA.SetFont(80, "Lucida Console");
  m_listRxRTA.AddString(new TraceMessage(TraceEvent::None, "REAL-TIME AGENT TRACE LOG")); 
  m_listRxRTA.setLimit(500);
  m_listRxRTA.enableLimit(TRUE);
  //m_LogOut.setToDisk(TRUE, "default_log.txt");

  return TRUE;
}



// RTA message in function
void CRTADlg::ShowRTAMsg (BYTE *msgBuf) {

  PRJDATA *pJ;
  rtah_msg_t *pRTA;
    

  pJ   = (PRJDATA *) (void *) msgBuf;
  pRTA = (rtah_msg_t *) (void *) pJ->szNames;

  // Lets process the message some if we know it
  if ((pRTA->header.split.fixed & 0x80) == 0) {
    if (m_CheckTaskSwitch.GetCheck() == BST_CHECKED) {
      this->OutLine ("[RTX TSK SW] : 0x%08X", pRTA->header.raw);
    }
  }
  else {
    switch (pRTA->header.split.type) { 
      case RTA_CMD_CUSTOM_0:
        if (m_showCustom[0].GetCheck() == BST_CHECKED) {
          OutLine ("[CUSTOM 0  ] 0x%08X", pRTA->header.raw);
          OutArray ((unsigned char *)&pRTA->data[0], pRTA->header.split.length);
        }
      break;
      case RTA_CMD_CUSTOM_1:
        if (m_showCustom[1].GetCheck() == BST_CHECKED) {
          this->OutLine ("[TRACE     ] %s", 
                      (unsigned char *)&pRTA->data[0]);
          //OutLine ("[CUSTOM 1  ] 0x%08X", pRTA->header.raw);
          //OutArray ((unsigned char *)&pRTA->data[0], pRTA->header.split.length);
        }
      break;
      case RTA_CMD_CUSTOM_2:
        if (m_showCustom[2].GetCheck() == BST_CHECKED) {
          this->OutLine ("[STATE     ] Clk: 0x%08X, Phase: %01X", 
              pRTA->data[0],
              pRTA->data[1]);
          //OutLine ("[CUSTOM 2  ] 0x%08X", pRTA->header.raw);
          //OutArray ((unsigned char *)&pRTA->data[0], pRTA->header.split.length);
        }
      break;
      case RTA_CMD_CUSTOM_3:
        if (m_showCustom[3].GetCheck() == BST_CHECKED) {
          OutLine ("[CUSTOM 3  ] 0x%08X", pRTA->header.raw);
          OutArray ((unsigned char *)&pRTA->data[0], pRTA->header.split.length);
        }
      break;
      case RTA_CMD_CUSTOM_4:
        if (m_showCustom[4].GetCheck() == BST_CHECKED) {
          OutLine ("[CUSTOM 4  ] 0x%08X", pRTA->header.raw);
          OutArray ((unsigned char *)&pRTA->data[0], pRTA->header.split.length);
        }
      break;
      case RTA_CMD_CUSTOM_5:
        if (m_showCustom[5].GetCheck() == BST_CHECKED) {
          OutLine ("[CUSTOM 5  ] 0x%08X", pRTA->header.raw);
          OutArray ((unsigned char *)&pRTA->data[0], pRTA->header.split.length);
        }
      break;
      case RTA_CMD_CUSTOM_6:
        if (m_showCustom[6].GetCheck() == BST_CHECKED) {
          OutLine ("[CUSTOM 6  ] 0x%08X", pRTA->header.raw);
          OutArray ((unsigned char *)&pRTA->data[0], pRTA->header.split.length);
        }
      break;
      case RTA_CMD_CUSTOM_7:
        if (m_showCustom[7].GetCheck() == BST_CHECKED) {
          OutLine ("[CUSTOM 7  ] 0x%08X", pRTA->header.raw);
          OutArray ((unsigned char *)&pRTA->data[0], pRTA->header.split.length);
        }
      break;
      case RTA_CMD_CUSTOM_8:
        if (m_showCustom[8].GetCheck() == BST_CHECKED) {
          OutLine ("[CUSTOM 8  ] 0x%08X", pRTA->header.raw);
          OutArray ((unsigned char *)&pRTA->data[0], pRTA->header.split.length);
        }
      break;
      case RTA_CMD_CUSTOM_9:
        if (m_showCustom[9].GetCheck() == BST_CHECKED) {
          OutLine ("[CUSTOM 9  ] 0x%08X", pRTA->header.raw);
          OutArray ((unsigned char *)&pRTA->data[0], pRTA->header.split.length);
        }
      break;
      case RTA_CMD_CUSTOM_10:
        if (m_showCustom[10].GetCheck() == BST_CHECKED) {
          OutLine ("[CUSTOM 10 ] 0x%08X", pRTA->header.raw);
          OutArray ((unsigned char *)&pRTA->data[0], pRTA->header.split.length);
        }
      break;
      case RTA_CMD_CUSTOM_11:
        if (m_showCustom[11].GetCheck() == BST_CHECKED) {
          OutLine ("[CUSTOM 11 ] 0x%08X", pRTA->header.raw);
          OutArray ((unsigned char *)&pRTA->data[0], pRTA->header.split.length);
        }
      break;
      case RTA_CMD_CUSTOM_12:
        if (m_showCustom[12].GetCheck() == BST_CHECKED) {
          OutLine ("[CUSTOM 12 ] 0x%08X", pRTA->header.raw);
          OutArray ((unsigned char *)&pRTA->data[0], pRTA->header.split.length);
        }
      break;
      case RTA_CMD_CUSTOM_13:
        if (m_showCustom[13].GetCheck() == BST_CHECKED) {
          OutLine ("[CUSTOM 13 ] 0x%08X", pRTA->header.raw);
          OutArray ((unsigned char *)&pRTA->data[0], pRTA->header.split.length);
        }
      break;
      case RTA_CMD_CUSTOM_14:
        if (m_showCustom[14].GetCheck() == BST_CHECKED) {
          OutLine ("[CUSTOM 14 ] 0x%08X", pRTA->header.raw);
          OutArray ((unsigned char *)&pRTA->data[0], pRTA->header.split.length);
        }
      break;
      case RTA_CMD_CUSTOM_15:
        if (m_showCustom[15].GetCheck() == BST_CHECKED) {
          OutLine ("[CUSTOM 15 ] 0x%08X", pRTA->header.raw);
          OutArray ((unsigned char *)&pRTA->data[0], pRTA->header.split.length);
        }
      break;

      default:
      break; // default
    } // switch
  } // if

  // Print raw data
  if (this->m_CheckRaw.GetCheck() == BST_CHECKED) {
    this->OutLine ("[RAW       ] tcplen=%d route=0x%02X type=0x%02X seq=0x%02X intlen=0x%02X", 
                    pJ->nLen, 
                    pJ->nCode, 
                    pRTA->header.split.type, 
                    pRTA->header.split.seq_no, 
                    pRTA->header.split.length);
    // Print data from DCC message
    OutArray((unsigned char *)&pRTA->data[0], pRTA->header.split.length); 
  }

}

void CRTADlg::OutArray (unsigned char *data, unsigned int len) {
  char bstr[1028];
  char *pbstr;
  unsigned int i;
  int ibstr;

  // Print array data 
    pbstr = bstr;
    for ( i = 0 ; i < len ; ++i )  {
      if ((i % 8) == 0)  {
        if (i == 0) ibstr = sprintf (pbstr, "            ");
        else        ibstr = sprintf (pbstr, "\n            ");
        pbstr += ibstr;
      }
      ibstr  = sprintf (pbstr, " %02X", data[i]);
      pbstr += ibstr;
    }
    sprintf (pbstr, "\n");
    this->OutLine (bstr);
}

#if 0
void CRTADlg::OutLine (char *fmt, ...)  {
  char  szB [1028];
  va_list    argp;
  int           n;
  int         nLength;

  if (fmt == NULL)  {            // used to clear edit control
    m_listRxRTA.ResetContent();
    return;
  }

  va_start (argp, fmt);
  if (argp == NULL)  {         //  || argp[0] == 0)  {
    n = min (strlen (fmt), sizeof (szB) - 4);
    memcpy (szB, fmt, n);
  }
  else  {
    n = _vsnprintf (szB, sizeof (szB) - 4, fmt, argp);
  }
  szB [n] = 0;

  if (n > 0 && szB [n - 1] == '\n') --n;
  if (n > 0 && szB [n - 1] == '\r') --n;
  szB [n]   = 0;


  m_listRxRTA.InsertString(-1, szB);                  // Put text in listbox
  nLength = m_listRxRTA.GetCount();                   // Select last item in list
  if (nLength > 0) m_listRxRTA.SetCurSel(nLength-1);
  
  // Remove items when list getting too long
  if (nLength > 1000) {
    m_listRxRTA.DeleteString(0);
  }
}
#else
void CRTADlg::OutLine (char *fmt, ...)  {
  char  szB [1028];
  va_list    argp;
  int n;

  va_start (argp, fmt);
  if (argp == NULL)  {         //  || argp[0] == 0)  {
    n = min (strlen (fmt), sizeof (szB) - 4);
    memcpy (szB, fmt, n);
  }
  else  {
    n = _vsnprintf (szB, sizeof (szB) - 4, fmt, argp);
  }
  szB [n] = 0;

  //if (n > 0 && szB [n - 1] == '\n') --n;
  //if (n > 0 && szB [n - 1] == '\r') --n;
  //szB [n++] = '\r';
  //szB [n++] = '\n';
  //szB [n]   = 0;

  m_listRxRTA.AddString(new TraceMessage(TraceEvent::None, szB)); 

}
#endif


void CRTADlg::OnBnClickedButtonclearwindow()
{
  m_listRxRTA.ResetContent();
}

void CRTADlg::OnBnClickedCheckcust1()
{
  // TODO: Add your control notification handler code here
}
