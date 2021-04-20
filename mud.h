#ifndef _MUD_H_
#define _MUD_H_

#include <string.h>
#include <stddef.h>
#include <sys/types.h>

/*
 *  FORMAT IDs - Must be unique!
 *               format of ID is:  0xLLFFSSSS
 *			   where:    LL       = lab identifier
 *			               FF     = format identifier
 *			                 RR   = revision
 *					   SS = section identifier
 */

/*
 *  Lab identifiers
 */

#define MUD_LAB_ALL_ID		(0x01000000)
#define MUD_LAB_TRI_ID		(0x02000000)
#define MUD_LAB_RAL_ID		(0x03000000)
#define MUD_LAB_PSI_ID		(0x04000000)
/*
 *  Format identifiers
 */
#define MUD_FMT_ALL_ID		(MUD_LAB_ALL_ID|0x00010000)
#define MUD_FMT_GEN_ID		(MUD_LAB_ALL_ID|0x00020000)
#define MUD_FMT_TRI_TD_ID	(MUD_LAB_TRI_ID|0x00010000)
#define MUD_FMT_TRI_TI_ID	(MUD_LAB_TRI_ID|0x00020000)
/*
 *  ALL Format identifiers
 */
#define	MUD_SEC_ID		(MUD_FMT_ALL_ID|0x00000001)
#define	MUD_SEC_FIXED_ID	(MUD_FMT_ALL_ID|0x00000002)
#define	MUD_SEC_GRP_ID		(MUD_FMT_ALL_ID|0x00000003)
#define	MUD_SEC_EOF_ID		(MUD_FMT_ALL_ID|0x00000004)
#define	MUD_SEC_CMT_ID		(MUD_FMT_ALL_ID|0x00000005)

#define	MUD_GRP_CMT_ID		MUD_SEC_CMT_ID
/*
 *  GEN Format identifiers
 */
#define	MUD_SEC_GEN_RUN_DESC_ID	    (MUD_FMT_GEN_ID|0x00000001)
#define	MUD_SEC_GEN_HIST_HDR_ID	    (MUD_FMT_GEN_ID|0x00000002)
#define	MUD_SEC_GEN_HIST_DAT_ID	    (MUD_FMT_GEN_ID|0x00000003)
#define	MUD_SEC_GEN_SCALER_ID	    (MUD_FMT_GEN_ID|0x00000004)
#define	MUD_SEC_GEN_IND_VAR_ID	    (MUD_FMT_GEN_ID|0x00000005)
#define MUD_SEC_GEN_ARRAY_ID        (MUD_FMT_GEN_ID|0x00000007)

#define	MUD_GRP_GEN_HIST_ID	    (MUD_FMT_GEN_ID|0x00000002)
#define	MUD_GRP_GEN_SCALER_ID	    (MUD_FMT_GEN_ID|0x00000004)
#define	MUD_GRP_GEN_IND_VAR_ID	    (MUD_FMT_GEN_ID|0x00000005)
#define	MUD_GRP_GEN_IND_VAR_ARR_ID  (MUD_FMT_GEN_ID|0x00000006)
/*
 *  TRI_TD Format identifiers
 */
#define	MUD_SEC_TRI_TD_HIST_ID	    (MUD_FMT_TRI_TD_ID|0x00000002)

#define	MUD_GRP_TRI_TD_HIST_ID	    (MUD_FMT_TRI_TD_ID|0x00000002)
#define	MUD_GRP_TRI_TD_SCALER_ID    (MUD_FMT_TRI_TD_ID|0x00000004)
/*
 *  TRI_TI Format identifiers
 */
#define	MUD_SEC_TRI_TI_RUN_DESC_ID  (MUD_FMT_TRI_TI_ID|0x00000001)
#define	MUD_SEC_TRI_TI_HIST_ID	    (MUD_FMT_TRI_TI_ID|0x00000002)

#define	MUD_GRP_TRI_TI_HIST_ID	    MUD_SEC_TRI_TI_HIST_ID
/*
 *  RAL Format identifiers
 */
#define	MUD_SEC_RAL_RUN_DESC_ID     (MUD_FMT_RAL_ID|0x00000001)
#define	MUD_SEC_RAL_HIST_ID         (MUD_FMT_RAL_ID|0x00000002)

#define	MUD_GRP_RAL_HIST_ID         MUD_SEC_RAL_HIST_ID
/*
 *  Add new format identifiers above this comment
 */

/*
 *  c_utils.h,  Defines for C utilities
 */

typedef int (*MUD_PROC)();

typedef enum {
    MUD_ENCODE = 0,
    MUD_DECODE = 1,
    MUD_FREE = 2,
    MUD_GET_SIZE = 3,
    MUD_SHOW = 4
} MUD_OPT;

typedef enum {
    MUD_ONE = 1,
    MUD_ALL = 2,
    MUD_GRP = 3
} MUD_IO_OPT;

typedef struct {
    struct _MUD_SEC*	pNext;	    /* pointer to next section */
    unsigned long	size;		    /* total size occupied by the section */
    unsigned long	secID;		    /* Ident of section type */
    unsigned long	instanceID;	    /* Instance ID of section type */
    unsigned long	sizeOf;		    /* sizeof struct (used for FORTRAN) */
    MUD_PROC	proc;		    /* section handling procedure */
} MUD_CORE;

typedef struct _MUD_INDEX {
    struct _MUD_INDEX*	pNext;	    /* pointer to next section */
    unsigned long	offset;		    /* offset from end of group section */
    unsigned long	secID;		    /* Ident of section type */
    unsigned long	instanceID;	    /* Instance ID of section type */
} MUD_INDEX;

typedef struct _SEEK_ENTRY {
    struct _SEEK_ENTRY* pNext;
    unsigned long secID;
    unsigned long instanceID;
} SEEK_ENTRY;

/*
 *  (Normally) for internal use only
 */
typedef struct {
    char* buf;
    int     pos;
    unsigned int size;
} BUF;

typedef struct _MUD_SEC {
    MUD_CORE	core;
} MUD_SEC;

/* First section of all MUD format files */
typedef struct {
    MUD_CORE	core;
    unsigned long	fileSize;
    unsigned long	formatID;
} MUD_SEC_FIXED;

/* Generalized group section */
typedef struct _MUD_SEC_GRP {
    MUD_CORE	core;
    unsigned long	num;		/* number of group members */
    unsigned long	memSize;
    MUD_INDEX*	pMemIndex;
    MUD_SEC*	pMem;		/* pointer to list of group members */
    long pos;
    struct _MUD_SEC_GRP* pParent;
} MUD_SEC_GRP;

/* Section indicating EOF */
typedef struct {
    MUD_CORE	core;
} MUD_SEC_EOF;

typedef struct {
    MUD_CORE	core;
    unsigned long  ID;
    unsigned long  prevReplyID;
    unsigned long  nextReplyID;
    unsigned long    time;
    char*   author;
    char*   title;
    char*   comment;
} MUD_SEC_CMT;


typedef struct {
    MUD_CORE	core;
    unsigned long	exptNumber;
    unsigned long	runNumber;
    unsigned long	timeBegin;
    unsigned long	timeEnd;
    unsigned long	elapsedSec;
    char*	title;
    char*	lab;
    char*	area;
    char*	method;
    char*	apparatus;
    char*	insert;
    char*	sample;
    char*	orient;
    char*	das;
    char*	experimenter;
    char*	temperature;
    char*	field;
} MUD_SEC_GEN_RUN_DESC;

/* Generic histogram header structure */
typedef struct {
    MUD_CORE	core;
    unsigned long	histType;
    unsigned long	nBytes;
    unsigned long	nBins;
    unsigned long	bytesPerBin;
    unsigned long	fsPerBin;
    unsigned long	t0_ps;
    unsigned long	t0_bin;
    unsigned long	goodBin1;
    unsigned long	goodBin2;
    unsigned long	bkgd1;
    unsigned long	bkgd2;
    unsigned long	nEvents;
    char*	title;
} MUD_SEC_GEN_HIST_HDR;

/* Generic (packed,integer) histogram data structure */
typedef struct {
    MUD_CORE	core;
    unsigned long	nBytes;
    char*	pData;		    /* pointer to the histogram data */
} MUD_SEC_GEN_HIST_DAT;

/* Generic (packed) array data structure */
typedef struct {
    MUD_CORE	core;
    unsigned long      num;		/* number of elements */
    unsigned long	elemSize;	/* size of element in bytes */
    unsigned long      type;		/* 1=integer, 2=real, 3=string */
    unsigned long	hasTime;	/* TRUE if there is time data */
    unsigned long	nBytes;         /* bytes in pData - needed for packing */
    char*	pData;		/* pointer to the array data */
    unsigned long*	pTime;		/* pointer to time data */
} MUD_SEC_GEN_ARRAY;

typedef struct {
    MUD_CORE	core;
    unsigned long	counts[2];
    char*	label;
} MUD_SEC_GEN_SCALER;

typedef struct {
    MUD_CORE	core;
    double	low;
    double	high;
    double	mean;
    double	stddev;
    double	skewness;
    char*	name;
    char*       description;
    char*	units;
} MUD_SEC_GEN_IND_VAR;

typedef struct {
    MUD_CORE	core;
    unsigned long	exptNumber;
    unsigned long	runNumber;
    unsigned long	timeBegin;
    unsigned long	timeEnd;
    unsigned long	elapsedSec;
    char*	title;
    char*	lab;
    char*	area;
    char*	method;
    char*	apparatus;
    char*	insert;
    char*	sample;
    char*	orient;
    char*	das;
    char*	experimenter;
    char*	subtitle;
    char*	comment1;
    char*	comment2;
    char*	comment3;
} MUD_SEC_TRI_TI_RUN_DESC;

#define MUD_pNext( pM )		(((MUD_SEC*)pM)->core.pNext)
#define MUD_sizeOf( pM )	(((MUD_SEC*)pM)->core.sizeOf)
#define MUD_size( pM )		(((MUD_SEC*)pM)->core.size)
#define MUD_secID( pM )		(((MUD_SEC*)pM)->core.secID)
#define MUD_instanceID( pM )	(((MUD_SEC*)pM)->core.instanceID)

#define _swap2bytes(s)       ((s>>8)+(s<<8))
#define _swap4bytes(l)       ((l>>24)+(l<<24)+((l&0xff0000)>>8)+((l&0xff00)<<8))

#define bdecode_obj( b, p, s )      memcpy( p, b, s )
#define bencode_obj( b, p, s )      memcpy( b, p, s )
#define bdecode_1( b, p )           memcpy( p, b, 1 )
#define bencode_1( b, p )           memcpy( b, p, 1 )

#define	_decode_obj( b, p, s )	    memcpy( p, &b->buf[b->pos], s );\
				    b->pos+=s, b->size+=s
#define	_encode_obj( b, p, s )	    memcpy( &b->buf[b->pos], p, s );\
				    b->pos+=s, b->size+=s

#define decode_2( b, p )            bdecode_2(&b->buf[b->pos],p);\
				    b->pos+=2, b->size+=2
#define encode_2( b, p )            bencode_2(&b->buf[b->pos],p);\
				    b->pos+=2, b->size+=2

#define decode_4( b, p )            bdecode_4(&b->buf[b->pos],p);\
				    b->pos+=4, b->size+=4
#define encode_4( b, p )            bencode_4(&b->buf[b->pos],p);\
				    b->pos+=4, b->size+=4

#define decode_8( b, p )            bdecode_8(&b->buf[b->pos],p);\
				    b->pos+=8, b->size+=8
#define encode_8( b, p )            bencode_8(&b->buf[b->pos],p);\
				    b->pos+=8, b->size+=8

#define decode_packed( b, p, n )    bdecode_packed(&b->buf[b->pos],p,n);\
				    b->pos+=n, b->size+=n
#define encode_packed( b, p, n )    bencode_packed(&b->buf[b->pos],p,n);\
				    b->pos+=n, b->size+=n

#define _buf_pos( b )		    b->pos
#define _buf_addr( b )		    &b->buf[b->pos]
#define _set_buf_pos( b, pos )	    b->pos = pos
#define _incr_buf_pos( b, incr )    b->pos += incr

/* mud.c */
FILE *MUD_openInput( char *inFile );
FILE *MUD_openOutput( char *outFile );
void decode_str( BUF *pB , char **ps );
void encode_str( BUF *pB , char **ps );
void MUD_free( void* pMUD );
unsigned long MUD_encode( BUF *pBuf , void* pMUD , MUD_IO_OPT io_opt );
void* MUD_decode( BUF *pBuf );
int MUD_getSize( void* pMUD );
void MUD_show( void* pMUD , MUD_IO_OPT io_opt );
unsigned long MUD_writeEnd( FILE *fout );
unsigned long MUD_writeFile( FILE *fout, void* pMUD_head );
unsigned long MUD_write( FILE *fout , void* pMUD , MUD_IO_OPT io_opt );
unsigned long MUD_writeGrpStart( FILE *fout, MUD_SEC_GRP *pMUD_parentGrp , MUD_SEC_GRP *pMUD_grp , int numMems );
void addIndex( MUD_SEC_GRP *pMUD_grp , void* pMUD );
unsigned long MUD_writeGrpMem( FILE *fout, MUD_SEC_GRP *pMUD_grp , void* pMUD );
unsigned long MUD_writeGrpEnd( FILE *fout, MUD_SEC_GRP *pMUD_grp );
void* MUD_readFile( FILE *fin );
void* MUD_read( FILE *fin, MUD_IO_OPT io_opt );
MUD_SEC* MUD_peekCore( FILE *fin );
void* MUD_search( void* pMUD_head, ...);
int MUD_fseek( FILE *fio, ...);
MUD_SEC *fseekNext( FILE *fio, MUD_SEC_GRP *pMUD_parent, unsigned long secID, unsigned long instanceID );
int MUD_fseekFirst( FILE *fio );
void MUD_add( void** ppMUD_head, void* pMUD_new );
int MUD_totSize( void* pMUD );
void MUD_addToGroup( MUD_SEC_GRP *pMUD_grp, void* pMUD );
void MUD_assignCore( MUD_SEC *pMUD1, MUD_SEC *pMUD2 );
int MUD_CORE_proc( MUD_OPT op, BUF *pBuf, MUD_SEC *pMUD );
int MUD_INDEX_proc( MUD_OPT op, BUF *pBuf, MUD_INDEX *pMUD );

/* mud_encode.c */
void bdecode_2( void *b, void *p );
void bencode_2( void *b, void *p );
void bdecode_4( void *b, void *p );
void bencode_4( void *b, void *p );
void bdecode_8( void *b, void *p );
void bencode_8( void *b, void *p );
void bencode_float( char *buf, float *fp );
void encode_float( BUF *pBuf, float *fp );
void bdecode_float( char *buf, float *fp );
void decode_float( BUF *pBuf, float *fp );
void bencode_double( char *buf, double *dp );
void encode_double( BUF *pBuf, double *fp );
void bdecode_double( char *buf, double *dp );
void decode_double( BUF *pBuf, double *fp );

/* mud_new.c */
MUD_SEC *MUD_new( unsigned long secID, unsigned long instanceID );

/* mud_all.c */
int MUD_SEC_proc( MUD_OPT op, BUF *pBuf, MUD_SEC *pMUD );
int MUD_SEC_EOF_proc( MUD_OPT op, BUF *pBuf, MUD_SEC_EOF *pMUD );
int MUD_SEC_FIXED_proc( MUD_OPT op, BUF *pBuf, MUD_SEC_FIXED *pMUD );
int MUD_SEC_GRP_proc( MUD_OPT op, BUF *pBuf, MUD_SEC_GRP *pMUD );
int MUD_SEC_CMT_proc( MUD_OPT op, BUF *pBuf, MUD_SEC_CMT *pMUD );

/* mud_gen.c */
int MUD_SEC_GEN_RUN_DESC_proc( MUD_OPT op, BUF *pBuf, MUD_SEC_GEN_RUN_DESC *pMUD );
int MUD_SEC_GEN_HIST_HDR_proc( MUD_OPT op, BUF *pBuf, MUD_SEC_GEN_HIST_HDR *pMUD );
int MUD_SEC_GEN_HIST_DAT_proc( MUD_OPT op, BUF *pBuf, MUD_SEC_GEN_HIST_DAT *pMUD );
int MUD_SEC_GEN_SCALER_proc( MUD_OPT op, BUF *pBuf, MUD_SEC_GEN_SCALER *pMUD );
int MUD_SEC_GEN_IND_VAR_proc( MUD_OPT op, BUF *pBuf, MUD_SEC_GEN_IND_VAR *pMUD );
int MUD_SEC_GEN_ARRAY_proc( MUD_OPT op, BUF *pBuf, MUD_SEC_GEN_ARRAY *pMUD );
int MUD_SEC_GEN_HIST_pack( int num, int inBinSize, void* inHist, int outBinSize, void* outHist );
int MUD_SEC_GEN_HIST_unpack( int num, int inBinSize, void* inHist, int outBinSize, void* outHist );

/* mud_tri_ti.c */
int MUD_SEC_TRI_TI_RUN_DESC_proc( MUD_OPT op, BUF *pBuf, MUD_SEC_TRI_TI_RUN_DESC *pMUD );

/* gmf_time.c */
void GMF_MKTIME( time_t* out, long* input );
void GMF_TIME( time_t* out );
void GMF_LOCALTIME( time_t* in, long *out );

/* mud_friendly.c */
int MUD_openRead( char* filename, unsigned long* pType );
int MUD_openWrite( char* filename, unsigned long type );
int MUD_closeRead( int fd );
int MUD_closeWrite( int fd );

int MUD_getRunDesc( int fd, unsigned long* pType );
int MUD_getExptNumber( int fd, unsigned long* pExptNumber );
int MUD_getRunNumber( int fd, unsigned long* pRunNumber );
int MUD_getElapsedSec( int fd, unsigned long* pElapsedSec );
int MUD_getTimeBegin( int fd, unsigned long* TimeBegin );
int MUD_getTimeEnd( int fd, unsigned long* TimeEnd );
int MUD_getTitle( int fd, char* title );
int MUD_getLab( int fd, char* lab );
int MUD_getArea( int fd, char* area );
int MUD_getMethod( int fd, char* method );
int MUD_getApparatus( int fd, char* apparatus );
int MUD_getInsert( int fd, char* insert );
int MUD_getSample( int fd, char* sample );
int MUD_getOrient( int fd, char* orient );
int MUD_getDas( int fd, char* das );
int MUD_getExperimenter( int fd, char* experimenter );
int MUD_getTemperature( int fd, char* temperature );
int MUD_getField( int fd, char* field );
int MUD_getSubtitle( int fd, char* subtitle );
int MUD_getComment1( int fd, char* comment1 );
int MUD_getComment2( int fd, char* comment2 );
int MUD_getComment3( int fd, char* comment3 );

int MUD_setRunDesc( int fd, unsigned long type );
int MUD_setExptNumber( int fd, unsigned long exptNumber );
int MUD_setRunNumber( int fd, unsigned long runNumber );
int MUD_setElapsedSec( int fd, unsigned long elapsedSec );
int MUD_setTimeBegin( int fd, unsigned long timeBegin );
int MUD_setTimeEnd( int fd, unsigned long timeEnd );
int MUD_setTitle( int fd, char* title );
int MUD_setLab( int fd, char* lab );
int MUD_setArea( int fd, char* area );
int MUD_setMethod( int fd, char* method );
int MUD_setApparatus( int fd, char* apparatus );
int MUD_setInsert( int fd, char* insert );
int MUD_setSample( int fd, char* sample );
int MUD_setOrient( int fd, char* orient );
int MUD_setDas( int fd, char* das );
int MUD_setExperimenter( int fd, char* experimenter );
int MUD_setTemperature( int fd, char* temperature );
int MUD_setField( int fd, char* field );
int MUD_setSubtitle( int fd, char* subtitle );
int MUD_setComment1( int fd, char* comment1 );
int MUD_setComment2( int fd, char* comment2 );
int MUD_setComment3( int fd, char* comment3 );

int MUD_getComments( int fd, unsigned long* pType, unsigned long* pNum );
int MUD_getCommentPrev( int fd, int num, unsigned long* pPrev );
int MUD_getCommentNext( int fd, int num, unsigned long* pNext );
int MUD_getCommentTime( int fd, int num, unsigned long* pTime );
int MUD_getCommentAuthor( int fd, int num, char* author );
int MUD_getCommentTitle( int fd, int num, char* title );
int MUD_getCommentBody( int fd, int num, char* body );

int MUD_setComments( int fd, unsigned long type, unsigned long num );
int MUD_setCommentPrev( int fd, int num, unsigned long prev );
int MUD_setCommentNext( int fd, int num, unsigned long next );
int MUD_setCommentTime( int fd, int num, unsigned long time );
int MUD_setCommentAuthor( int fd, int num, char* author );
int MUD_setCommentTitle( int fd, int num, char* title );
int MUD_setCommentBody( int fd, int num, char* body );

int MUD_getHists( int fd, unsigned long* pType, unsigned long* pNum );
int MUD_getHistType( int fd, int num, unsigned long* pType );
int MUD_getHistNumBytes( int fd, int num, unsigned long* pNumBytes );
int MUD_getHistNumBins( int fd, int num, unsigned long* pNumBins );
int MUD_getHistBytesPerBin( int fd, int num, unsigned long* pBytesPerBin );
int MUD_getHistFsPerBin( int fd, int num, unsigned long* pFsPerBin );
int MUD_getHistT0_Ps( int fd, int num, unsigned long* pT0_ps );
int MUD_getHistT0_Bin( int fd, int num, unsigned long* pT0_bin );
int MUD_getHistGoodBin1( int fd, int num, unsigned long* pGoodBin1 );
int MUD_getHistGoodBin2( int fd, int num, unsigned long* pGoodBin2 );
int MUD_getHistBkgd1( int fd, int num, unsigned long* pBkgd1 );
int MUD_getHistBkgd2( int fd, int num, unsigned long* pBkgd2 );
int MUD_getHistNumEvents( int fd, int num, unsigned long* pNumEvents );
int MUD_getHistTitle( int fd, int num, char* title );
int MUD_getHistData( int fd, int num, void* pData );
int MUD_getHistpData( int fd, int num, void** ppData );

int MUD_setHists( int fd, unsigned long type, unsigned long num );
int MUD_setHistType( int fd, int num, unsigned long type );
int MUD_setHistNumBytes( int fd, int num, unsigned long numBytes );
int MUD_setHistNumBins( int fd, int num, unsigned long numBins );
int MUD_setHistBytesPerBin( int fd, int num, unsigned long bytesPerBin );
int MUD_setHistFsPerBin( int fd, int num, unsigned long fsPerBin );
int MUD_setHistT0_Ps( int fd, int num, unsigned long t0_ps );
int MUD_setHistT0_Bin( int fd, int num, unsigned long t0_bin );
int MUD_setHistGoodBin1( int fd, int num, unsigned long goodBin1 );
int MUD_setHistGoodBin2( int fd, int num, unsigned long goodBin2 );
int MUD_setHistBkgd1( int fd, int num, unsigned long bkgd1 );
int MUD_setHistBkgd2( int fd, int num, unsigned long bkgd2 );
int MUD_setHistNumEvents( int fd, int num, unsigned long numEvents );
int MUD_setHistTitle( int fd, int num, char* title );
int MUD_setHistData( int fd, int num, void* pData );
int MUD_setHistpData( int fd, int num, void* pData );

int MUD_pack( int num, int inBinSize, void* inArray, int outBinSize, void* outArray );
int MUD_unpack( int num, int inBinSize, void* inArray, int outBinSize, void* outArray );

int MUD_getScalers( int fd, unsigned long* pType, unsigned long* pNum );
int MUD_getScalerLabel( int fd, int num, char* label );
int MUD_getScalerCounts( int fd, int num, unsigned long* pCounts );

int MUD_setScalers( int fd, unsigned long type, unsigned long num );
int MUD_setScalerLabel( int fd, int num, char* label );
int MUD_setScalerCounts( int fd, int num, unsigned long* pCounts );

int MUD_getIndVars( int fd, unsigned long* pType, unsigned long* pNum );
int MUD_getIndVarLow( int fd, int num, double* pLow );
int MUD_getIndVarHigh( int fd, int num, double* pHigh );
int MUD_getIndVarMean( int fd, int num, double* pMean );
int MUD_getIndVarStddev( int fd, int num, double* pStddev );
int MUD_getIndVarSkewness( int fd, int num, double* pSkewness );
int MUD_getIndVarName( int fd, int num, char* name );
int MUD_getIndVarDescription( int fd, int num, char* description );
int MUD_getIndVarUnits( int fd, int num, char* units );
int MUD_getIndVarNumData( int fd, int num, unsigned long* pNumData );
int MUD_getIndVarElemSize( int fd, int num, unsigned long* pElemSize );
int MUD_getIndVarDataType( int fd, int num, unsigned long* pDataType );
int MUD_getIndVarHasTime( int fd, int num, unsigned long* pHasTime );
int MUD_getIndVarData( int fd, int num, void* pData  );
int MUD_getIndVarTimeData( int fd, int num, unsigned long* pTimeData  );
int MUD_getIndVarpData( int fd, int num, void** ppData  );
int MUD_getIndVarpTimeData( int fd, int num, unsigned long** ppTimeData  );

int MUD_setIndVars( int fd, unsigned long type, unsigned long num );
int MUD_setIndVarLow( int fd, int num, double low );
int MUD_setIndVarHigh( int fd, int num, double high );
int MUD_setIndVarMean( int fd, int num, double mean );
int MUD_setIndVarStddev( int fd, int num, double stddev );
int MUD_setIndVarSkewness( int fd, int num, double skewness );
int MUD_setIndVarName( int fd, int num, char* name );
int MUD_setIndVarDescription( int fd, int num, char* description );
int MUD_setIndVarUnits( int fd, int num, char* units );
int MUD_setIndVarNumData( int fd, int num, unsigned long numData );
int MUD_setIndVarElemSize( int fd, int num, unsigned long elemSize );
int MUD_setIndVarDataType( int fd, int num, unsigned long dataType );
int MUD_setIndVarHasTime( int fd, int num, unsigned long hasTime );
int MUD_setIndVarData( int fd, int num, void* pData );
int MUD_setIndVarTimeData( int fd, int num, unsigned long* pTimeData );
int MUD_setIndVarpData( int fd, int num, void* pData );
int MUD_setIndVarpTimeData( int fd, int num, unsigned long* pTimeData );
#endif

