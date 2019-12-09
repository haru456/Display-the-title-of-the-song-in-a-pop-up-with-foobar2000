// foo_Popup.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "SDK-2015-08-03\foobar2000\SDK\foobar2000.h"

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーから使用されていない部分を除外します。
// Windows ヘッダー ファイル:
#include <windows.h>
#include <Winbase.h>

#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#include <shellapi.h>
#include <locale.h>

#define IDI_POPUP 500
#define TASKTRAY_ID 5
#define MAX_TITLE  254
#define MAX_TEXT  63

DECLARE_COMPONENT_VERSION(
"Popup plugin",
"1.1",
"Popup appears when new song plays"
);
VALIDATE_COMPONENT_FILENAME("foo_Popup.dll");

NOTIFYICONDATA nid;
HWND hfoobar;
int byteLenGet(char* multi);

int main()
{
    return 0;
}

class CPlayback : public play_callback_impl_base {
public:
	virtual unsigned int get_flags(void);
	virtual void on_playback_new_track(metadb_handle_ptr track);
};
CPlayback* pb;

class myInit :public initquit {
public:
	virtual void on_init();
	virtual void on_quit();
};

static initquit_factory_t<myInit> ifc;

void myInit::on_init()
{
	Shell_NotifyIcon(NIM_DELETE, &nid);

	if (!pb) {
		pb = new CPlayback();
	}
	hfoobar = FindWindow(foobarのクラス名を入力, NULL);
	if (hfoobar != NULL) {
		console::info("sucsess");
	}
	HICON hIcon;
	hIcon = LoadIcon(0, MAKEINTRESOURCE(IDI_POPUP));
	ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hfoobar;
	nid.uID = TASKTRAY_ID;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_USER + 1;
	nid.hIcon = hIcon;
	nid.uTimeout = 10000;
	lstrcpy(nid.szTip, L"現在の曲");
	Shell_NotifyIcon(NIM_ADD, &nid);
}
void myInit::on_quit() {
	Shell_NotifyIcon(NIM_DELETE, &nid);
	delete pb;
}

unsigned int CPlayback::get_flags(void)
{
	return flag_on_playback_new_track;
}
void CPlayback::on_playback_new_track(metadb_handle_ptr track)
{
	TCHAR btitle[MAX_TITLE] = L"";
	TCHAR btext[MAX_TEXT] = L"";
	//TCHAR bartist[350]=L""　アーティストも表示させるならコメント外してください
	file_info_impl info;
	track->get_info(info);

	if (info.meta_exists("TITLE")) {
		//char型配列をTCHAR型配列に変換
		char* title;
		//char* artist;
		char* album;
		int title_len, album_len;

		title = (char *)info.meta_get("TITLE", 0);
		title_len = byteLenGet(title);
		//artist = (char *)info.meta_get("ARTIST", 0);
		album = (char *)info.meta_get("ALBUM", 0);
		album_len = byteLenGet(album);
		MultiByteToWideChar(CP_UTF8, 0, title, title_len, btext, title_len+1);
		MultiByteToWideChar(CP_UTF8, 0, album, album_len, btitle, album_len+1);
		//MultiByteToWideChar(CP_UTF8, 0, artist, 350, bartist, (sizeof bartist) / 2);
		
		ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.uID = TASKTRAY_ID;
		nid.uFlags = NIF_INFO;
		nid.hWnd = hfoobar;
		lstrcpy(nid.szInfoTitle, btitle); //タイトルに表示する文字
		lstrcpy(nid.szInfo, btext);	//テキストに表示する文字
		Shell_NotifyIcon(NIM_MODIFY, &nid);
	}
}

//文字数カウント
int byteLenGet(char* multi)
{
	int i,len;
	int bytelen; 	//バイト数
	int charlen;	//文字数

	/* ロケールをjpn(日本)にする */
	setlocale(LC_CTYPE, "jan");

	bytelen = (int)strlen(multi);

	charlen = 0;
	i = 0;
	for (i = 0; i<bytelen; i++) {
		/* 各文字ごとのバイト数を取得する */
		len = mblen(&multi[i], MB_CUR_MAX);
		if (len != -1) {
			i += len;
			charlen++;
		}
		else {
			break;
		}
	}
	return bytelen;
}
