; Listing generated by Microsoft (R) Optimizing Compiler Version 16.00.30319.01 

	TITLE	C:\Users\Joachim Hofer\Documents\mostlyharmless\mistymist\i4k_OGL\src\Parameter.cpp
	.686P
	.XMM
	include listing.inc
	.model	flat

INCLUDELIB LIBCMT
INCLUDELIB OLDNAMES

EXTRN	__imp__CloseClipboard@0:PROC
EXTRN	__imp__SetClipboardData@8:PROC
EXTRN	__imp__EmptyClipboard@0:PROC
EXTRN	__imp__OpenClipboard@4:PROC
EXTRN	__imp__GlobalUnlock@4:PROC
EXTRN	__imp__GlobalLock@4:PROC
EXTRN	__imp__GlobalAlloc@8:PROC
; Function compile flags: /Ogsp
; File c:\users\joachim hofer\documents\mostlyharmless\mistymist\i4k_ogl\src\parameter.cpp
;	COMDAT ?CopyToClipboard@@YG_NPAD@Z
_TEXT	SEGMENT
tv66 = -8						; size = 4
_bRet$ = -1						; size = 1
?CopyToClipboard@@YG_NPAD@Z PROC			; CopyToClipboard, COMDAT
; _strText$ = eax

; 7    : {

	push	ebp
	mov	ebp, esp
	push	ecx
	push	ecx
	push	esi
	mov	esi, eax

; 8    : 	bool bRet = true;

	mov	BYTE PTR _bRet$[ebp], 1

; 9    : 
; 10   : 	if (strlen(strText) > 0)

	lea	ecx, DWORD PTR [eax+1]
$LL9@CopyToClip:
	mov	dl, BYTE PTR [eax]
	inc	eax
	test	dl, dl
	jne	SHORT $LL9@CopyToClip
	sub	eax, ecx
	je	SHORT $LN12@CopyToClip

; 11   : 	{
; 12   : 		HGLOBAL		hGlobalMemory;
; 13   : 		LPVOID		lpGlobalMemory;
; 14   : 
; 15   : 		// allocate global memory
; 16   : 		hGlobalMemory = GlobalAlloc(GHND, strlen(strText) + 1);

	mov	eax, esi
	lea	edx, DWORD PTR [eax+1]
$LL10@CopyToClip:
	mov	cl, BYTE PTR [eax]
	inc	eax
	test	cl, cl
	jne	SHORT $LL10@CopyToClip
	sub	eax, edx
	push	edi
	inc	eax
	push	eax
	push	66					; 00000042H
	call	DWORD PTR __imp__GlobalAlloc@8
	mov	edi, eax

; 17   : 		if (hGlobalMemory)

	test	edi, edi
	je	SHORT $LN13@CopyToClip

; 18   : 		{
; 19   : 			// lock block to get a pointer to it
; 20   : 			lpGlobalMemory = GlobalLock(hGlobalMemory);

	push	edi
	call	DWORD PTR __imp__GlobalLock@4

; 21   : 			// copy string to the block
; 22   : 			lpGlobalMemory = strcpy((char *)lpGlobalMemory, strText);

	sub	eax, esi
$LL7@CopyToClip:
	mov	cl, BYTE PTR [esi]
	mov	BYTE PTR [eax+esi], cl
	inc	esi
	test	cl, cl
	jne	SHORT $LL7@CopyToClip

; 23   : 			// free memory
; 24   : 			GlobalUnlock(hGlobalMemory);

	push	edi
	call	DWORD PTR __imp__GlobalUnlock@4

; 25   : 
; 26   : 			// open clipboard for task
; 27   : 			if (OpenClipboard(0))

	push	0
	call	DWORD PTR __imp__OpenClipboard@4
	test	eax, eax
	je	SHORT $LN13@CopyToClip

; 28   : 			{
; 29   : 				EmptyClipboard();

	call	DWORD PTR __imp__EmptyClipboard@0

; 30   : 				// try writing if successful
; 31   : 				if (!SetClipboardData(CF_TEXT, hGlobalMemory))

	push	edi
	push	1
	call	DWORD PTR __imp__SetClipboardData@8
	test	eax, eax
	jne	SHORT $LN11@CopyToClip

; 32   : 				{
; 33   : 					// error writing to clipboard
; 34   : 					bRet = false;

	mov	BYTE PTR _bRet$[ebp], al
$LN11@CopyToClip:

; 35   : 				}
; 36   : 				CloseClipboard();

	call	DWORD PTR __imp__CloseClipboard@0
$LN13@CopyToClip:
	pop	edi
$LN12@CopyToClip:

; 37   : 			}
; 38   : 		}
; 39   : 	}
; 40   : 	return bRet;

	mov	al, BYTE PTR _bRet$[ebp]
	pop	esi

; 41   : }

	leave
	ret	0
?CopyToClipboard@@YG_NPAD@Z ENDP			; CopyToClipboard
_TEXT	ENDS
PUBLIC	??_C@_08CDEONOBK@?$CFd?3?$CF?42f?5?$AA@		; `string'
PUBLIC	??1Parameter@@QAE@XZ				; Parameter::~Parameter
EXTRN	_sprintf:PROC
EXTRN	__imp__midiInClose@4:PROC
EXTRN	__imp__midiInStop@4:PROC
EXTRN	__imp__midiInReset@4:PROC
EXTRN	__fltused:DWORD
;	COMDAT ??_C@_08CDEONOBK@?$CFd?3?$CF?42f?5?$AA@
CONST	SEGMENT
??_C@_08CDEONOBK@?$CFd?3?$CF?42f?5?$AA@ DB '%d:%.2f ', 00H ; `string'
; Function compile flags: /Ogsp
CONST	ENDS
;	COMDAT ??1Parameter@@QAE@XZ
_TEXT	SEGMENT
_string$ = -1552					; size = 1536
_tmpString$ = -16					; size = 12
_par$66643 = -4						; size = 4
??1Parameter@@QAE@XZ PROC				; Parameter::~Parameter, COMDAT
; _this$ = ecx

; 70   : {

	push	ebp
	mov	ebp, esp
	sub	esp, 1552				; 00000610H
	push	ebx
	push	esi
	push	edi
	mov	ebx, ecx

; 71   : 	char string[NUM_PARAMETERS*12];
; 72   : 	char tmpString[12];
; 73   : 
; 74   : 	/* Stop midi */
; 75   : 	for (unsigned int devID = 0; devID < numDevices; devID++)

	xor	edi, edi
	cmp	DWORD PTR [ebx+640], edi
	jbe	SHORT $LN5@Parameter
	lea	esi, DWORD PTR [ebx+644]
$LL7@Parameter:

; 76   : 	{
; 77   : 		MMRESULT rc;
; 78   : 		rc = midiInReset(midiInDevice[devID]);

	push	DWORD PTR [esi]
	call	DWORD PTR __imp__midiInReset@4

; 79   : 		rc = midiInStop(midiInDevice[devID]);

	push	DWORD PTR [esi]
	call	DWORD PTR __imp__midiInStop@4

; 80   : 		rc = midiInClose(midiInDevice[devID]);

	push	DWORD PTR [esi]
	call	DWORD PTR __imp__midiInClose@4
	inc	edi
	add	esi, 4
	cmp	edi, DWORD PTR [ebx+640]
	jb	SHORT $LL7@Parameter
$LN5@Parameter:

; 81   : 	}
; 82   : 
; 83   : 	/* Save to clipboard... */
; 84   : 	string[0] = 0;
; 85   : 	for (int par = 0; par < NUM_PARAMETERS; par++)

	xor	eax, eax
	mov	BYTE PTR _string$[ebp], 0
	mov	DWORD PTR _par$66643[ebp], eax
$LL4@Parameter:

; 86   : 	{
; 87   : 		if (changed[par])

	cmp	BYTE PTR [ebx+eax+512], 0
	je	SHORT $LN3@Parameter

; 88   : 		{
; 89   : 			sprintf(tmpString, "%d:%.2f ", par, value[par]);

	fld	DWORD PTR [ebx+eax*4]
	push	ecx
	push	ecx
	fstp	QWORD PTR [esp]
	push	eax
	lea	eax, DWORD PTR _tmpString$[ebp]
	push	OFFSET ??_C@_08CDEONOBK@?$CFd?3?$CF?42f?5?$AA@
	push	eax
	call	_sprintf

; 90   : 			strcat(string, tmpString);

	lea	eax, DWORD PTR _tmpString$[ebp]
	add	esp, 20					; 00000014H
	mov	edx, eax
$LL14@Parameter:
	mov	cl, BYTE PTR [eax]
	inc	eax
	test	cl, cl
	jne	SHORT $LL14@Parameter
	lea	edi, DWORD PTR _string$[ebp]
	sub	eax, edx
	dec	edi
$LL15@Parameter:
	mov	cl, BYTE PTR [edi+1]
	inc	edi
	test	cl, cl
	jne	SHORT $LL15@Parameter
	mov	ecx, eax
	shr	ecx, 2
	mov	esi, edx
	rep movsd
	mov	ecx, eax
	mov	eax, DWORD PTR _par$66643[ebp]
	and	ecx, 3
	rep movsb
$LN3@Parameter:

; 81   : 	}
; 82   : 
; 83   : 	/* Save to clipboard... */
; 84   : 	string[0] = 0;
; 85   : 	for (int par = 0; par < NUM_PARAMETERS; par++)

	inc	eax
	mov	DWORD PTR _par$66643[ebp], eax
	cmp	eax, 128				; 00000080H
	jl	SHORT $LL4@Parameter

; 91   : 		}
; 92   : 	}
; 93   : 
; 94   : 	CopyToClipboard(string);

	lea	eax, DWORD PTR _string$[ebp]
	call	?CopyToClipboard@@YG_NPAD@Z		; CopyToClipboard
	pop	edi
	pop	esi
	pop	ebx

; 95   : }

	leave
	ret	0
??1Parameter@@QAE@XZ ENDP				; Parameter::~Parameter
_TEXT	ENDS
PUBLIC	?getParam@Parameter@@QAEMHM@Z			; Parameter::getParam
; Function compile flags: /Ogsp
;	COMDAT ?getParam@Parameter@@QAEMHM@Z
_TEXT	SEGMENT
_index$ = 8						; size = 4
_defaultValue$ = 12					; size = 4
?getParam@Parameter@@QAEMHM@Z PROC			; Parameter::getParam, COMDAT
; _this$ = ecx

; 98   : {

	push	ebp
	mov	ebp, esp

; 99   : 	if (!changed[index])

	mov	eax, DWORD PTR _index$[ebp]
	cmp	BYTE PTR [eax+ecx+512], 0
	jne	SHORT $LN1@getParam

; 100  : 	{
; 101  : 		value[index] = defaultValue;

	fld	DWORD PTR _defaultValue$[ebp]
	fstp	DWORD PTR [ecx+eax*4]
$LN1@getParam:

; 102  : 	}
; 103  : 
; 104  : 	return value[index];

	fld	DWORD PTR [ecx+eax*4]

; 105  : }

	pop	ebp
	ret	8
?getParam@Parameter@@QAEMHM@Z ENDP			; Parameter::getParam
_TEXT	ENDS
PUBLIC	__real@3c010204
PUBLIC	?MidiInProc@Parameter@@SGXPAUHMIDIIN__@@IKKK@Z	; Parameter::MidiInProc
;	COMDAT __real@3c010204
CONST	SEGMENT
__real@3c010204 DD 03c010204r			; 0.00787402
; Function compile flags: /Ogsp
CONST	ENDS
;	COMDAT ?MidiInProc@Parameter@@SGXPAUHMIDIIN__@@IKKK@Z
_TEXT	SEGMENT
_hMidiIn$ = 8						; size = 4
tv84 = 12						; size = 4
_wMsg$ = 12						; size = 4
_dwInstance$ = 16					; size = 4
_dwParam1$ = 20						; size = 4
_dwParam2$ = 24						; size = 4
?MidiInProc@Parameter@@SGXPAUHMIDIIN__@@IKKK@Z PROC	; Parameter::MidiInProc, COMDAT

; 108  : {

	push	ebp
	mov	ebp, esp

; 109  : 	if (wMsg == MIM_DATA)

	cmp	DWORD PTR _wMsg$[ebp], 963		; 000003c3H
	jne	SHORT $LN1@MidiInProc

; 110  : 	{
; 111  : 		int first = (dwParam1 >> 8) & 255;

	mov	ecx, DWORD PTR _dwParam1$[ebp]
	mov	eax, ecx
	shr	eax, 8
	mov	edx, 255				; 000000ffH
	and	eax, edx

; 112  : 		int second = (dwParam1 >> 16) & 255;
; 113  : 		params.changed[first] = true;
; 114  : 		params.value[first] = (float)second / 127.0f;

	shr	ecx, 16					; 00000010H
	and	ecx, edx
	mov	DWORD PTR tv84[ebp], ecx
	fild	DWORD PTR tv84[ebp]
	mov	BYTE PTR ?params@@3VParameter@@A[eax+512], 1
	fmul	DWORD PTR __real@3c010204
	fstp	DWORD PTR ?params@@3VParameter@@A[eax*4]
$LN1@MidiInProc:

; 115  : 	}
; 116  : }

	pop	ebp
	ret	20					; 00000014H
?MidiInProc@Parameter@@SGXPAUHMIDIIN__@@IKKK@Z ENDP	; Parameter::MidiInProc
_TEXT	ENDS
PUBLIC	__real@00000000
PUBLIC	??0Parameter@@QAE@XZ				; Parameter::Parameter
EXTRN	__imp__midiInStart@4:PROC
EXTRN	__imp__midiInAddBuffer@12:PROC
EXTRN	__imp__midiInPrepareHeader@12:PROC
EXTRN	__imp__midiInOpen@20:PROC
EXTRN	__imp__midiInGetNumDevs@0:PROC
;	COMDAT __real@00000000
CONST	SEGMENT
__real@00000000 DD 000000000r			; 0
; Function compile flags: /Ogsp
CONST	ENDS
;	COMDAT ??0Parameter@@QAE@XZ
_TEXT	SEGMENT
tv368 = -8						; size = 4
_devID$66625 = -4					; size = 4
??0Parameter@@QAE@XZ PROC				; Parameter::Parameter, COMDAT
; _this$ = ecx

; 44   : {

	push	ebp
	mov	ebp, esp
	push	ecx
	push	ecx
	push	ebx
	mov	ebx, ecx

; 45   : 	/* Initialize values */
; 46   : 	for (int i = 0; i < NUM_PARAMETERS; i++)

	xor	eax, eax
$LL7@Parameter@2:

; 47   : 	{
; 48   : 		value[i] = 0.0f;

	fldz
	fstp	DWORD PTR [ebx+eax*4]

; 49   : 		changed[i] = false;

	mov	BYTE PTR [ebx+eax+512], 0
	inc	eax
	cmp	eax, 128				; 00000080H
	jl	SHORT $LL7@Parameter@2

; 50   : 	}
; 51   : 
; 52   : 	/* Open Midi device */
; 53   : 	numDevices = midiInGetNumDevs();

	call	DWORD PTR __imp__midiInGetNumDevs@0

; 54   : 	if (numDevices > MAX_NUM_DEVICES) numDevices = MAX_NUM_DEVICES;

	push	32					; 00000020H
	pop	ecx
	mov	DWORD PTR [ebx+640], eax
	cmp	eax, ecx
	jbe	SHORT $LN4@Parameter@2
	mov	DWORD PTR [ebx+640], ecx
$LN4@Parameter@2:

; 55   : 	for (unsigned int devID = 0; devID < numDevices; devID++)

	and	DWORD PTR _devID$66625[ebp], 0
	cmp	DWORD PTR [ebx+640], 0
	jbe	SHORT $LN1@Parameter@2
	push	esi
	lea	eax, DWORD PTR [ebx+2820]
	push	edi
	lea	edi, DWORD PTR [ebx+644]
	lea	esi, DWORD PTR [ebx+772]
	mov	DWORD PTR tv368[ebp], eax
$LL14@Parameter@2:

; 56   : 	{
; 57   : 		MMRESULT rc;
; 58   : 		rc = midiInOpen(&(midiInDevice[devID]), devID, (DWORD_PTR)MidiInProc, NULL, CALLBACK_FUNCTION);

	push	196608					; 00030000H
	push	0
	push	OFFSET ?MidiInProc@Parameter@@SGXPAUHMIDIIN__@@IKKK@Z ; Parameter::MidiInProc
	push	DWORD PTR _devID$66625[ebp]
	push	edi
	call	DWORD PTR __imp__midiInOpen@20

; 59   : 		/* Create input buffers */
; 60   : 		midiInHeader[devID].lpData = (LPSTR)midiData[devID];

	mov	eax, DWORD PTR tv368[ebp]

; 61   : 		midiInHeader[devID].dwBufferLength = MIDI_DATA_SIZE;
; 62   : 		midiInHeader[devID].dwFlags = 0;

	and	DWORD PTR [esi+16], 0

; 63   : 		rc = midiInPrepareHeader(midiInDevice[devID], &midiInHeader[devID], sizeof(midiInHeader[devID]));

	push	64					; 00000040H
	push	esi
	mov	DWORD PTR [esi], eax
	mov	DWORD PTR [esi+4], 1024			; 00000400H
	push	DWORD PTR [edi]
	call	DWORD PTR __imp__midiInPrepareHeader@12

; 64   : 		rc = midiInAddBuffer(midiInDevice[devID], &midiInHeader[devID], sizeof(midiInHeader[devID]));

	push	64					; 00000040H
	push	esi
	push	DWORD PTR [edi]
	call	DWORD PTR __imp__midiInAddBuffer@12

; 65   : 		rc = midiInStart(midiInDevice[devID]);

	push	DWORD PTR [edi]
	call	DWORD PTR __imp__midiInStart@4
	inc	DWORD PTR _devID$66625[ebp]
	mov	eax, DWORD PTR _devID$66625[ebp]
	add	DWORD PTR tv368[ebp], 1024		; 00000400H
	add	edi, 4
	add	esi, 64					; 00000040H
	cmp	eax, DWORD PTR [ebx+640]
	jb	SHORT $LL14@Parameter@2
	pop	edi
	pop	esi
$LN1@Parameter@2:

; 66   : 	}
; 67   : }

	mov	eax, ebx
	pop	ebx
	leave
	ret	0
??0Parameter@@QAE@XZ ENDP				; Parameter::Parameter
_TEXT	ENDS
EXTRN	_atexit:PROC
; Function compile flags: /Ogsp
;	COMDAT ??__Eparams@@YAXXZ
text$yc	SEGMENT
??__Eparams@@YAXXZ PROC					; `dynamic initializer for 'params'', COMDAT

; 118  : Parameter params;

	mov	ecx, OFFSET ?params@@3VParameter@@A	; params
	call	??0Parameter@@QAE@XZ			; Parameter::Parameter
	push	OFFSET ??__Fparams@@YAXXZ		; `dynamic atexit destructor for 'params''
	call	_atexit
	pop	ecx
	ret	0
??__Eparams@@YAXXZ ENDP					; `dynamic initializer for 'params''
; Function compile flags: /Ogsp
text$yc	ENDS
;	COMDAT ??__Fparams@@YAXXZ
text$yd	SEGMENT
??__Fparams@@YAXXZ PROC					; `dynamic atexit destructor for 'params'', COMDAT
	mov	ecx, OFFSET ?params@@3VParameter@@A	; params
	jmp	??1Parameter@@QAE@XZ			; Parameter::~Parameter
??__Fparams@@YAXXZ ENDP					; `dynamic atexit destructor for 'params''
text$yd	ENDS
PUBLIC	?params@@3VParameter@@A				; params
_BSS	SEGMENT
?params@@3VParameter@@A DB 08b04H DUP (?)		; params
_BSS	ENDS
CRT$XCU	SEGMENT
_params$initializer$ DD FLAT:??__Eparams@@YAXXZ
CRT$XCU	ENDS
END