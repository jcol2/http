#pragma once
#define UNICODE
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <PathCch.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#pragma comment(lib, "ws2_32.lib")

typedef intptr_t ssize_t;



// atoi_yy.c
// https://github.com/ibireme/c_numconv_benchmark/blob/master/src/atoi/atoi_yy.c



typedef enum {
    atoi_result_suc = 0,
    atoi_result_fail = 1,
    atoi_result_overflow = 2,
} atoi_result;

/* compiler builtin check (clang) */
#ifndef yy_has_builtin
#   ifdef __has_builtin
#       define yy_has_builtin(x) __has_builtin(x)
#   else
#       define yy_has_builtin(x) 0
#   endif
#endif

/* compiler attribute check (gcc/clang) */
#ifndef yy_has_attribute
#   ifdef __has_attribute
#       define yy_has_attribute(x) __has_attribute(x)
#   else
#       define yy_has_attribute(x) 0
#   endif
#endif

/* inline */
#ifndef yy_inline
#   if _MSC_VER >= 1200
#       define yy_inline __forceinline
#   elif defined(_MSC_VER)
#       define yy_inline __inline
#   elif yy_has_attribute(always_inline) || __GNUC__ >= 4
#       define yy_inline __inline__ __attribute__((always_inline))
#   elif defined(__clang__) || defined(__GNUC__)
#       define yy_inline __inline__
#   elif defined(__cplusplus) || (__STDC__ >= 1 && __STDC_VERSION__ >= 199901L)
#       define yy_inline inline
#   else
#       define yy_inline
#   endif
#endif

/* likely */
#ifndef yy_likely
#   if yy_has_builtin(__builtin_expect) || __GNUC__ >= 4
#       define yy_likely(expr) __builtin_expect(!!(expr), 1)
#   else
#       define yy_likely(expr) (expr)
#   endif
#endif

/* unlikely */
#ifndef yy_unlikely
#   if yy_has_builtin(__builtin_expect) || __GNUC__ >= 4
#       define yy_unlikely(expr) __builtin_expect(!!(expr), 0)
#   else
#       define yy_unlikely(expr) (expr)
#   endif
#endif

#define repeat_in_1_8(x) { x(1) x(2) x(3) x(4) x(5) x(6) x(7) x(8) }

#define repeat_in_1_17(x) { x(1) x(2) x(3) x(4) x(5) x(6) x(7) \
                            x(8) x(9) x(10) x(11) x(12) x(13) x(14) x(15) \
                            x(16) x(17) }

#define repeat_in_1_18(x) { x(1) x(2) x(3) x(4) x(5) x(6) x(7) \
                            x(8) x(9) x(10) x(11) x(12) x(13) x(14) x(15) \
                            x(16) x(17) x(18) }

/** Digit type */
typedef uint8_t digi_type;

/** Digit: '0'. */
static const digi_type DIGI_TYPE_ZERO       = 1 << 0;

/** Digit: [1-9]. */
static const digi_type DIGI_TYPE_NONZERO    = 1 << 1;

/** Minus sign (negative): '-'. */
static const digi_type DIGI_TYPE_NEG        = 1 << 3;


/** Digit type table (generate with misc/make_tables.c) */
static const digi_type digi_table[256] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x08, 0x10, 0x00,
    0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/** Match a character with specified type. */
static yy_inline bool digi_is_type(uint8_t d, digi_type type) {
    return (digi_table[d] & type) != 0;
}

/** Match a none zero digit: [1-9] */
static yy_inline bool digi_is_nonzero(uint8_t d) {
    return digi_is_type(d, DIGI_TYPE_NONZERO);
}

/** Match a digit: [0-9] */
static yy_inline bool digi_is_digit(uint8_t d) {
    return digi_is_type(d, DIGI_TYPE_ZERO | DIGI_TYPE_NONZERO);
}

uint32_t atoi_u32_yy(const char *str, size_t len, char **endptr, atoi_result *res) {
    if (yy_unlikely(!digi_is_nonzero(*str))) {
        if (*str == '0' && !digi_is_digit(str[1])) {
            *endptr = (char *)str + 1;
            *res = atoi_result_suc;
        } else {
            *endptr = (char *)str;
            *res = atoi_result_fail;
        }
        return 0;
    }
    
    const char *cur = str;
    uint32_t val = (uint32_t)(*cur - '0'), add;
    *res = atoi_result_suc;
    
#define expr_int(i) \
    if (yy_likely((add = (uint32_t)(cur[i] - '0')) <= 9)) val = add + val * 10; \
    else goto digi_end_##i;
    repeat_in_1_8(expr_int);
#undef expr_int
    goto digi_more;
    
#define expr_end(i) \
    digi_end_##i: *endptr = (char *)cur + i; return val;
    repeat_in_1_8(expr_end)
#undef expr_end
    
digi_more:
    cur += 9;
    if (digi_is_digit(*cur)) {
        add = *cur++ - '0';
        if ((val > UINT32_MAX / 10) ||
            ((val == UINT32_MAX / 10) && (add > UINT32_MAX % 10)) ||
            digi_is_digit(*cur)) {
            while (digi_is_digit(*cur)) cur++;
            *res = atoi_result_overflow;
            *endptr = (char *)cur;
            return UINT32_MAX;;
        } else {
            *endptr = (char *)cur;
            return val * 10 + add;
        }
    } else {
        *endptr = (char *)cur;
        return val;
    }
}



// Utf8 Validation



static uint32_t
Utf8Validate(char *Str, size_t StrLen)
{
 for (size_t I = 0; I < StrLen; ++I)
 {
  uint8_t C = Str[I];
  if (C == 0)
  {
   return 0;
  }
  else if (C <= 0x7F)
  {
   // do nothing
  }
  else if (C >= 0xC2 && C <= 0xDF)
  {
   if (++I >= StrLen)
   {
    return 0;
   }
   C = Str[I];
   if ((C & 0xC0) != 0x80)
   {
    return 0;
   }
  }
  else if (C == 0xE0)
  {
   if (++I >= StrLen)
   {
    return 0;
   }
   C = Str[I];
   if (((C & 0xC0) != 0x80) || (C >= 0x80 && C <= 0x9F))
   {
    return 0;
   }
   if (++I >= StrLen)
   {
    return 0;
   }
   C = Str[I];
   if ((C & 0xC0) != 0x80)
   {
    return 0;
   }
  }
  else if (C == 0xED)
  {
   if (++I >= StrLen)
   {
    return 0;
   }
   C = Str[I];
   if (C >= 0xA0)
   {
    return 0;
   }
   if (++I >= StrLen)
   {
    return 0;
   }
   C = Str[I];
   if ((C & 0xC0) != 0x80)
   {
    return 0;
   }
  }
  else if ((C >= 0xE1 && C <= 0xEC) || C == 0XEE || C == 0xEF)
  {
   if (++I >= StrLen)
   {
    return 0;
   }
   C = Str[I];
   if ((C & 0xC0) != 0x80)
   {
    return 0;
   }
   if (++I >= StrLen)
   {
    return 0;
   }
   C = Str[I];
   if ((C & 0xC0) != 0x80)
   {
    return 0;
   }
  }
  else if (C == 0xF0)
  {
   if (++I >= StrLen)
   {
    return 0;
   }
   C = Str[I];
   if (((C & 0xC0) != 0x80) || (C >= 0x80 && C <= 0x8F))
   {
    return 0;
   }
   if (++I >= StrLen)
   {
    return 0;
   }
   C = Str[I];
   if ((C & 0xC0) != 0x80)
   {
    return 0;
   }
   if (++I >= StrLen)
   {
    return 0;
   }
   C = Str[I];
   if ((C & 0xC0) != 0x80)
   {
    return 0;
   }
  }
  else if (C >= 0xF1 && C <= 0xF3)
  {
   if (++I >= StrLen)
   {
    return 0;
   }
   C = Str[I];
   if (((C & 0xC0) != 0x80))
   {
    return 0;
   }
   if (++I >= StrLen)
   {
    return 0;
   }
   C = Str[I];
   if ((C & 0xC0) != 0x80)
   {
    return 0;
   }
   if (++I >= StrLen)
   {
    return 0;
   }
   C = Str[I];
   if ((C & 0xC0) != 0x80)
   {
    return 0;
   }
  }
  else if (C == 0xF4)
  {
   if (++I >= StrLen)
   {
    return 0;
   }
   C = Str[I];
   if (C >= 0x90)
   {
    return 0;
   }
   if (++I >= StrLen)
   {
    return 0;
   }
   C = Str[I];
   if ((C & 0xC0) != 0x80)
   {
    return 0;
   }
   if (++I >= StrLen)
   {
    return 0;
   }
   C = Str[I];
   if ((C & 0xC0) != 0x80)
   {
    return 0;
   }
  }
  else
  {
   return 0;
  }
 }

 return 1;
}




// Mime



#define HttpArrLn(Arr) (sizeof(Arr) / sizeof((Arr)[0]))

// Table from:
// https://github.com/samuelneff/MimeTypeMap/blob/master/MimeTypeMap.cs
static const char *MimeTab[][641] = {
 {"323", "text/h323"},
 {"3g2", "video/3gpp2"},
 {"3gp", "video/3gpp"},
 {"3gp2", "video/3gpp2"},
 {"3gpp", "video/3gpp"},
 {"7z", "application/x-7z-compressed"},
 {"AAC", "audio/aac"},
 {"ADT", "audio/vnd.dlna.adts"},
 {"ADTS", "audio/aac"},
 {"AddIn", "text/xml"},
 {"IVF", "video/x-ivf"},
 {"SSISDeploymentManifest", "text/xml"},
 {"WLMP", "application/wlmoviemaker"},
 {"XOML", "text/plain"},
 {"aa", "audio/audible"},
 {"aaf", "application/octet-stream"},
 {"aax", "audio/vnd.audible.aax"},
 {"ac3", "audio/ac3"},
 {"aca", "application/octet-stream"},
 {"accda", "application/msaccess.addin"},
 {"accdb", "application/msaccess"},
 {"accdc", "application/msaccess.cab"},
 {"accde", "application/msaccess"},
 {"accdr", "application/msaccess.runtime"},
 {"accdt", "application/msaccess"},
 {"accdw", "application/msaccess.webapplication"},
 {"accft", "application/msaccess.ftemplate"},
 {"acx", "application/internet-property-stream"},
 {"ade", "application/msaccess"},
 {"adobebridge", "application/x-bridge-url"},
 {"adp", "application/msaccess"},
 {"afm", "application/octet-stream"},
 {"ai", "application/postscript"},
 {"aif", "audio/aiff"},
 {"aifc", "audio/aiff"},
 {"aiff", "audio/aiff"},
 {"air", "application/vnd.adobe.air-application-installer-package+zip"},
 {"amc", "application/mpeg"},
 {"anx", "application/annodex"},
 {"apk", "application/vnd.android.package-archive"},
 {"apng", "image/apng"},
 {"application", "application/x-ms-application"},
 {"art", "image/x-jg"},
 {"asa", "application/xml"},
 {"asax", "application/xml"},
 {"ascx", "application/xml"},
 {"asd", "application/octet-stream"},
 {"asf", "video/x-ms-asf"},
 {"ashx", "application/xml"},
 {"asi", "application/octet-stream"},
 {"asm", "text/plain"},
 {"asmx", "application/xml"},
 {"aspx", "application/xml"},
 {"asr", "video/x-ms-asf"},
 {"asx", "video/x-ms-asf"},
 {"atom", "application/atom+xml"},
 {"au", "audio/basic"},
 {"avci", "image/avci"},
 {"avcs", "image/avcs"},
 {"avi", "video/x-msvideo"},
 {"avif", "image/avif"},
 {"avifs", "image/avif-sequence"},
 {"axa", "audio/annodex"},
 {"axs", "application/olescript"},
 {"axv", "video/annodex"},
 {"bas", "text/plain"},
 {"bcpio", "application/x-bcpio"},
 {"bin", "application/octet-stream"},
 {"bmp", "image/bmp"},
 {"c", "text/plain"},
 {"cab", "application/octet-stream"},
 {"caf", "audio/x-caf"},
 {"calx", "application/vnd.ms-office.calx"},
 {"cat", "application/vnd.ms-pki.seccat"},
 {"cc", "text/plain"},
 {"cd", "text/plain"},
 {"cdda", "audio/aiff"},
 {"cdf", "application/x-cdf"},
 {"cer", "application/x-x509-ca-cert"},
 {"cfg", "text/plain"},
 {"chm", "application/octet-stream"},
 {"class", "application/x-java-applet"},
 {"clp", "application/x-msclip"},
 {"cmd", "text/plain"},
 {"cmx", "image/x-cmx"},
 {"cnf", "text/plain"},
 {"cod", "image/cis-cod"},
 {"config", "application/xml"},
 {"contact", "text/x-ms-contact"},
 {"coverage", "application/xml"},
 {"cpio", "application/x-cpio"},
 {"cpp", "text/plain"},
 {"crd", "application/x-mscardfile"},
 {"crl", "application/pkix-crl"},
 {"crt", "application/x-x509-ca-cert"},
 {"cs", "text/plain"},
 {"csdproj", "text/plain"},
 {"csh", "application/x-csh"},
 {"csproj", "text/plain"},
 {"css", "text/css"},
 {"csv", "text/csv"},
 {"cur", "application/octet-stream"},
 {"cxx", "text/plain"},
 {"czx", "application/x-czx"},
 {"dat", "application/octet-stream"},
 {"datasource", "application/xml"},
 {"dbproj", "text/plain"},
 {"dcr", "application/x-director"},
 {"def", "text/plain"},
 {"deploy", "application/octet-stream"},
 {"der", "application/x-x509-ca-cert"},
 {"dgml", "application/xml"},
 {"dib", "image/bmp"},
 {"dif", "video/x-dv"},
 {"dir", "application/x-director"},
 {"disco", "text/xml"},
 {"divx", "video/divx"},
 {"dll", "application/x-msdownload"},
 {"dll.config", "text/xml"},
 {"dlm", "text/dlm"},
 {"doc", "application/msword"},
 {"docm", "application/vnd.ms-word.document.macroEnabled.12"},
 {"docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
 {"dot", "application/msword"},
 {"dotm", "application/vnd.ms-word.template.macroEnabled.12"},
 {"dotx", "application/vnd.openxmlformats-officedocument.wordprocessingml.template"},
 {"dsp", "application/octet-stream"},
 {"dsw", "text/plain"},
 {"dtd", "text/xml"},
 {"dtsConfig", "text/xml"},
 {"dv", "video/x-dv"},
 {"dvi", "application/x-dvi"},
 {"dwf", "drawing/x-dwf"},
 {"dwg", "application/acad"},
 {"dwp", "application/octet-stream"},
 {"dxf", "application/x-dxf"},
 {"dxr", "application/x-director"},
 {"emf", "image/emf"},
 {"eml", "message/rfc822"},
 {"emz", "application/octet-stream"},
 {"eot", "application/vnd.ms-fontobject"},
 {"eps", "application/postscript"},
 {"es", "application/ecmascript"},
 {"etl", "application/etl"},
 {"etx", "text/x-setext"},
 {"evy", "application/envoy"},
 {"exe", "application/vnd.microsoft.portable-executable"},
 {"exe.config", "text/xml"},
 {"f4v", "video/mp4"},
 {"fdf", "application/vnd.fdf"},
 {"fif", "application/fractals"},
 {"filters", "application/xml"},
 {"fla", "application/octet-stream"},
 {"flac", "audio/flac"},
 {"flr", "x-world/x-vrml"},
 {"flv", "video/x-flv"},
 {"fsscript", "application/fsharp-script"},
 {"fsx", "application/fsharp-script"},
 {"generictest", "application/xml"},
 {"geojson", "application/geo+json"},
 {"gif", "image/gif"},
 {"gml", "application/gml+xml"},
 {"gpx", "application/gpx+xml"},
 {"group", "text/x-ms-group"},
 {"gsm", "audio/x-gsm"},
 {"gtar", "application/x-gtar"},
 {"gz", "application/x-gzip"},
 {"h", "text/plain"},
 {"hdf", "application/x-hdf"},
 {"hdml", "text/x-hdml"},
 {"heic", "image/heic"},
 {"heics", "image/heic-sequence"},
 {"heif", "image/heif"},
 {"heifs", "image/heif-sequence"},
 {"hhc", "application/x-oleobject"},
 {"hhk", "application/octet-stream"},
 {"hhp", "application/octet-stream"},
 {"hlp", "application/winhlp"},
 {"hpp", "text/plain"},
 {"hqx", "application/mac-binhex40"},
 {"hta", "application/hta"},
 {"htc", "text/x-component"},
 {"htm", "text/html"},
 {"html", "text/html"},
 {"htt", "text/webviewhtml"},
 {"hxa", "application/xml"},
 {"hxc", "application/xml"},
 {"hxd", "application/octet-stream"},
 {"hxe", "application/xml"},
 {"hxf", "application/xml"},
 {"hxh", "application/octet-stream"},
 {"hxi", "application/octet-stream"},
 {"hxk", "application/xml"},
 {"hxq", "application/octet-stream"},
 {"hxr", "application/octet-stream"},
 {"hxs", "application/octet-stream"},
 {"hxt", "text/html"},
 {"hxv", "application/xml"},
 {"hxw", "application/octet-stream"},
 {"hxx", "text/plain"},
 {"i", "text/plain"},
 {"ical", "text/calendar"},
 {"icalendar", "text/calendar"},
 {"ico", "image/x-icon"},
 {"ics", "text/calendar"},
 {"idl", "text/plain"},
 {"ief", "image/ief"},
 {"ifb", "text/calendar"},
 {"iii", "application/x-iphone"},
 {"inc", "text/plain"},
 {"inf", "application/octet-stream"},
 {"ini", "text/plain"},
 {"inl", "text/plain"},
 {"ins", "application/x-internet-signup"},
 {"ipa", "application/x-itunes-ipa"},
 {"ipg", "application/x-itunes-ipg"},
 {"ipproj", "text/plain"},
 {"ipsw", "application/x-itunes-ipsw"},
 {"iqy", "text/x-ms-iqy"},
 {"isma", "application/octet-stream"},
 {"ismv", "application/octet-stream"},
 {"isp", "application/x-internet-signup"},
 {"ite", "application/x-itunes-ite"},
 {"itlp", "application/x-itunes-itlp"},
 {"itms", "application/x-itunes-itms"},
 {"itpc", "application/x-itunes-itpc"},
 {"jar", "application/java-archive"},
 {"java", "application/octet-stream"},
 {"jck", "application/liquidmotion"},
 {"jcz", "application/liquidmotion"},
 {"jfif", "image/pjpeg"},
 {"jnlp", "application/x-java-jnlp-file"},
 {"jpb", "application/octet-stream"},
 {"jpe", "image/jpeg"},
 {"jpeg", "image/jpeg"},
 {"jpg", "image/jpeg"},
 {"js", "application/javascript"},
 {"json", "application/json"},
 {"jsx", "text/jscript"},
 {"jsxbin", "text/plain"},
 {"key", "application/vnd.apple.keynote"},
 {"latex", "application/x-latex"},
 {"library-ms", "application/windows-library+xml"},
 {"lit", "application/x-ms-reader"},
 {"loadtest", "application/xml"},
 {"lpk", "application/octet-stream"},
 {"lsf", "video/x-la-asf"},
 {"lst", "text/plain"},
 {"lsx", "video/x-la-asf"},
 {"lzh", "application/octet-stream"},
 {"m13", "application/x-msmediaview"},
 {"m14", "application/x-msmediaview"},
 {"m1v", "video/mpeg"},
 {"m2t", "video/vnd.dlna.mpeg-tts"},
 {"m2ts", "video/vnd.dlna.mpeg-tts"},
 {"m2v", "video/mpeg"},
 {"m3u", "audio/x-mpegurl"},
 {"m3u8", "audio/x-mpegurl"},
 {"m4a", "audio/m4a"},
 {"m4b", "audio/m4b"},
 {"m4p", "audio/m4p"},
 {"m4r", "audio/x-m4r"},
 {"m4v", "video/x-m4v"},
 {"mac", "image/x-macpaint"},
 {"mak", "text/plain"},
 {"man", "application/x-troff-man"},
 {"manifest", "application/x-ms-manifest"},
 {"map", "text/plain"},
 {"master", "application/xml"},
 {"mbox", "application/mbox"},
 {"mda", "application/msaccess"},
 {"mdb", "application/x-msaccess"},
 {"mde", "application/msaccess"},
 {"mdp", "application/octet-stream"},
 {"me", "application/x-troff-me"},
 {"mfp", "application/x-shockwave-flash"},
 {"mht", "message/rfc822"},
 {"mhtml", "message/rfc822"},
 {"mid", "audio/mid"},
 {"midi", "audio/mid"},
 {"mix", "application/octet-stream"},
 {"mk", "text/plain"},
 {"mk3d", "video/x-matroska-3d"},
 {"mka", "audio/x-matroska"},
 {"mkv", "video/x-matroska"},
 {"mmf", "application/x-smaf"},
 {"mno", "text/xml"},
 {"mny", "application/x-msmoney"},
 {"mod", "video/mpeg"},
 {"mov", "video/quicktime"},
 {"movie", "video/x-sgi-movie"},
 {"mp2", "video/mpeg"},
 {"mp2v", "video/mpeg"},
 {"mp3", "audio/mpeg"},
 {"mp4", "video/mp4"},
 {"mp4v", "video/mp4"},
 {"mpa", "video/mpeg"},
 {"mpe", "video/mpeg"},
 {"mpeg", "video/mpeg"},
 {"mpf", "application/vnd.ms-mediapackage"},
 {"mpg", "video/mpeg"},
 {"mpp", "application/vnd.ms-project"},
 {"mpv2", "video/mpeg"},
 {"mqv", "video/quicktime"},
 {"ms", "application/x-troff-ms"},
 {"msg", "application/vnd.ms-outlook"},
 {"msi", "application/octet-stream"},
 {"mso", "application/octet-stream"},
 {"mts", "video/vnd.dlna.mpeg-tts"},
 {"mtx", "application/xml"},
 {"mvb", "application/x-msmediaview"},
 {"mvc", "application/x-miva-compiled"},
 {"mxf", "application/mxf"},
 {"mxp", "application/x-mmxp"},
 {"nc", "application/x-netcdf"},
 {"nsc", "video/x-ms-asf"},
 {"numbers", "application/vnd.apple.numbers"},
 {"nws", "message/rfc822"},
 {"ocx", "application/octet-stream"},
 {"oda", "application/oda"},
 {"odb", "application/vnd.oasis.opendocument.database"},
 {"odc", "application/vnd.oasis.opendocument.chart"},
 {"odf", "application/vnd.oasis.opendocument.formula"},
 {"odg", "application/vnd.oasis.opendocument.graphics"},
 {"odh", "text/plain"},
 {"odi", "application/vnd.oasis.opendocument.image"},
 {"odl", "text/plain"},
 {"odm", "application/vnd.oasis.opendocument.text-master"},
 {"odp", "application/vnd.oasis.opendocument.presentation"},
 {"ods", "application/vnd.oasis.opendocument.spreadsheet"},
 {"odt", "application/vnd.oasis.opendocument.text"},
 {"oga", "audio/ogg"},
 {"ogg", "audio/ogg"},
 {"ogv", "video/ogg"},
 {"ogx", "application/ogg"},
 {"one", "application/onenote"},
 {"onea", "application/onenote"},
 {"onepkg", "application/onenote"},
 {"onetmp", "application/onenote"},
 {"onetoc", "application/onenote"},
 {"onetoc2", "application/onenote"},
 {"opus", "audio/ogg"},
 {"orderedtest", "application/xml"},
 {"osdx", "application/opensearchdescription+xml"},
 {"otf", "application/font-sfnt"},
 {"otg", "application/vnd.oasis.opendocument.graphics-template"},
 {"oth", "application/vnd.oasis.opendocument.text-web"},
 {"otp", "application/vnd.oasis.opendocument.presentation-template"},
 {"ots", "application/vnd.oasis.opendocument.spreadsheet-template"},
 {"ott", "application/vnd.oasis.opendocument.text-template"},
 {"oxps", "application/oxps"},
 {"oxt", "application/vnd.openofficeorg.extension"},
 {"p10", "application/pkcs10"},
 {"p12", "application/x-pkcs12"},
 {"p7b", "application/x-pkcs7-certificates"},
 {"p7c", "application/pkcs7-mime"},
 {"p7m", "application/pkcs7-mime"},
 {"p7r", "application/x-pkcs7-certreqresp"},
 {"p7s", "application/pkcs7-signature"},
 {"pages", "application/vnd.apple.pages"},
 {"pbm", "image/x-portable-bitmap"},
 {"pcast", "application/x-podcast"},
 {"pct", "image/pict"},
 {"pcx", "application/octet-stream"},
 {"pcz", "application/octet-stream"},
 {"pdf", "application/pdf"},
 {"pfb", "application/octet-stream"},
 {"pfm", "application/octet-stream"},
 {"pfx", "application/x-pkcs12"},
 {"pgm", "image/x-portable-graymap"},
 {"pic", "image/pict"},
 {"pict", "image/pict"},
 {"pkgdef", "text/plain"},
 {"pkgundef", "text/plain"},
 {"pko", "application/vnd.ms-pki.pko"},
 {"pls", "audio/scpls"},
 {"pma", "application/x-perfmon"},
 {"pmc", "application/x-perfmon"},
 {"pml", "application/x-perfmon"},
 {"pmr", "application/x-perfmon"},
 {"pmw", "application/x-perfmon"},
 {"png", "image/png"},
 {"pnm", "image/x-portable-anymap"},
 {"pnt", "image/x-macpaint"},
 {"pntg", "image/x-macpaint"},
 {"pnz", "image/png"},
 {"pot", "application/vnd.ms-powerpoint"},
 {"potm", "application/vnd.ms-powerpoint.template.macroEnabled.12"},
 {"potx", "application/vnd.openxmlformats-officedocument.presentationml.template"},
 {"ppa", "application/vnd.ms-powerpoint"},
 {"ppam", "application/vnd.ms-powerpoint.addin.macroEnabled.12"},
 {"ppm", "image/x-portable-pixmap"},
 {"pps", "application/vnd.ms-powerpoint"},
 {"ppsm", "application/vnd.ms-powerpoint.slideshow.macroEnabled.12"},
 {"ppsx", "application/vnd.openxmlformats-officedocument.presentationml.slideshow"},
 {"ppt", "application/vnd.ms-powerpoint"},
 {"pptm", "application/vnd.ms-powerpoint.presentation.macroEnabled.12"},
 {"pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
 {"prf", "application/pics-rules"},
 {"prm", "application/octet-stream"},
 {"prx", "application/octet-stream"},
 {"ps", "application/postscript"},
 {"psc1", "application/PowerShell"},
 {"psd", "application/octet-stream"},
 {"psess", "application/xml"},
 {"psm", "application/octet-stream"},
 {"psp", "application/octet-stream"},
 {"pst", "application/vnd.ms-outlook"},
 {"pub", "application/x-mspublisher"},
 {"pwz", "application/vnd.ms-powerpoint"},
 {"qht", "text/x-html-insertion"},
 {"qhtm", "text/x-html-insertion"},
 {"qt", "video/quicktime"},
 {"qti", "image/x-quicktime"},
 {"qtif", "image/x-quicktime"},
 {"qtl", "application/x-quicktimeplayer"},
 {"qxd", "application/octet-stream"},
 {"ra", "audio/x-pn-realaudio"},
 {"ram", "audio/x-pn-realaudio"},
 {"rar", "application/x-rar-compressed"},
 {"ras", "image/x-cmu-raster"},
 {"rat", "application/rat-file"},
 {"rc", "text/plain"},
 {"rc2", "text/plain"},
 {"rct", "text/plain"},
 {"rdlc", "application/xml"},
 {"reg", "text/plain"},
 {"resx", "application/xml"},
 {"rf", "image/vnd.rn-realflash"},
 {"rgb", "image/x-rgb"},
 {"rgs", "text/plain"},
 {"rm", "application/vnd.rn-realmedia"},
 {"rmi", "audio/mid"},
 {"rmp", "application/vnd.rn-rn_music_package"},
 {"rmvb", "application/vnd.rn-realmedia-vbr"},
 {"roff", "application/x-troff"},
 {"rpm", "audio/x-pn-realaudio-plugin"},
 {"rqy", "text/x-ms-rqy"},
 {"rtf", "application/rtf"},
 {"rtx", "text/richtext"},
 {"ruleset", "application/xml"},
 {"rvt", "application/octet-stream"},
 {"s", "text/plain"},
 {"safariextz", "application/x-safari-safariextz"},
 {"scd", "application/x-msschedule"},
 {"scr", "text/plain"},
 {"sct", "text/scriptlet"},
 {"sd2", "audio/x-sd2"},
 {"sdp", "application/sdp"},
 {"sea", "application/octet-stream"},
 {"searchConnector-ms", "application/windows-search-connector+xml"},
 {"setpay", "application/set-payment-initiation"},
 {"setreg", "application/set-registration-initiation"},
 {"settings", "application/xml"},
 {"sgimb", "application/x-sgimb"},
 {"sgml", "text/sgml"},
 {"sh", "application/x-sh"},
 {"shar", "application/x-shar"},
 {"shtml", "text/html"},
 {"sit", "application/x-stuffit"},
 {"sitemap", "application/xml"},
 {"skin", "application/xml"},
 {"skp", "application/x-koan"},
 {"sldm", "application/vnd.ms-powerpoint.slide.macroEnabled.12"},
 {"sldx", "application/vnd.openxmlformats-officedocument.presentationml.slide"},
 {"slk", "application/vnd.ms-excel"},
 {"sln", "text/plain"},
 {"slupkg-ms", "application/x-ms-license"},
 {"smd", "audio/x-smd"},
 {"smi", "application/octet-stream"},
 {"smx", "audio/x-smd"},
 {"smz", "audio/x-smd"},
 {"snd", "audio/basic"},
 {"snippet", "application/xml"},
 {"snp", "application/octet-stream"},
 {"sol", "text/plain"},
 {"sor", "text/plain"},
 {"spc", "application/x-pkcs7-certificates"},
 {"spl", "application/futuresplash"},
 {"spx", "audio/ogg"},
 {"sql", "application/sql"},
 {"src", "application/x-wais-source"},
 {"srf", "text/plain"},
 {"ssm", "application/streamingmedia"},
 {"sst", "application/vnd.ms-pki.certstore"},
 {"step", "application/step"},
 {"stl", "application/vnd.ms-pki.stl"},
 {"stp", "application/step"},
 {"sv4cpio", "application/x-sv4cpio"},
 {"sv4crc", "application/x-sv4crc"},
 {"svc", "application/xml"},
 {"svg", "image/svg+xml"},
 {"swf", "application/x-shockwave-flash"},
 {"t", "application/x-troff"},
 {"tar", "application/x-tar"},
 {"tcl", "application/x-tcl"},
 {"testrunconfig", "application/xml"},
 {"testsettings", "application/xml"},
 {"tex", "application/x-tex"},
 {"texi", "application/x-texinfo"},
 {"texinfo", "application/x-texinfo"},
 {"tgz", "application/x-compressed"},
 {"thmx", "application/vnd.ms-officetheme"},
 {"thn", "application/octet-stream"},
 {"tif", "image/tiff"},
 {"tiff", "image/tiff"},
 {"tlh", "text/plain"},
 {"tli", "text/plain"},
 {"toc", "application/octet-stream"},
 {"tr", "application/x-troff"},
 {"trm", "application/x-msterminal"},
 {"trx", "application/xml"},
 {"ts", "video/vnd.dlna.mpeg-tts"},
 {"tsv", "text/tab-separated-values"},
 {"ttf", "application/font-sfnt"},
 {"tts", "video/vnd.dlna.mpeg-tts"},
 {"txt", "text/plain"},
 {"u32", "application/octet-stream"},
 {"uls", "text/iuls"},
 {"user", "text/plain"},
 {"ustar", "application/x-ustar"},
 {"vb", "text/plain"},
 {"vbdproj", "text/plain"},
 {"vbk", "video/mpeg"},
 {"vbproj", "text/plain"},
 {"vbs", "text/vbscript"},
 {"vcf", "text/x-vcard"},
 {"vcproj", "application/xml"},
 {"vcs", "text/plain"},
 {"vcxproj", "application/xml"},
 {"vddproj", "text/plain"},
 {"vdp", "text/plain"},
 {"vdproj", "text/plain"},
 {"vdx", "application/vnd.ms-visio.viewer"},
 {"vml", "text/xml"},
 {"vscontent", "application/xml"},
 {"vsct", "text/xml"},
 {"vsd", "application/vnd.visio"},
 {"vsdx", "application/vnd.ms-visio.viewer"},
 {"vsi", "application/ms-vsi"},
 {"vsix", "application/vsix"},
 {"vsixlangpack", "text/xml"},
 {"vsixmanifest", "text/xml"},
 {"vsmdi", "application/xml"},
 {"vspscc", "text/plain"},
 {"vss", "application/vnd.visio"},
 {"vsscc", "text/plain"},
 {"vssettings", "text/xml"},
 {"vssscc", "text/plain"},
 {"vst", "application/vnd.visio"},
 {"vstemplate", "text/xml"},
 {"vsto", "application/x-ms-vsto"},
 {"vsw", "application/vnd.visio"},
 {"vsx", "application/vnd.visio"},
 {"vtt", "text/vtt"},
 {"vtx", "application/vnd.visio"},
 {"wasm", "application/wasm"},
 {"wav", "audio/wav"},
 {"wave", "audio/wav"},
 {"wax", "audio/x-ms-wax"},
 {"wbk", "application/msword"},
 {"wbmp", "image/vnd.wap.wbmp"},
 {"wcm", "application/vnd.ms-works"},
 {"wdb", "application/vnd.ms-works"},
 {"wdp", "image/vnd.ms-photo"},
 {"webarchive", "application/x-safari-webarchive"},
 {"webm", "video/webm"},
 {"webp", "image/webp"}, /* https://en.wikipedia.org/wiki/WebP */
 {"webtest", "application/xml"},
 {"wiq", "application/xml"},
 {"wiz", "application/msword"},
 {"wks", "application/vnd.ms-works"},
 {"wlpginstall", "application/x-wlpg-detect"},
 {"wlpginstall3", "application/x-wlpg3-detect"},
 {"wm", "video/x-ms-wm"},
 {"wma", "audio/x-ms-wma"},
 {"wmd", "application/x-ms-wmd"},
 {"wmf", "application/x-msmetafile"},
 {"wml", "text/vnd.wap.wml"},
 {"wmlc", "application/vnd.wap.wmlc"},
 {"wmls", "text/vnd.wap.wmlscript"},
 {"wmlsc", "application/vnd.wap.wmlscriptc"},
 {"wmp", "video/x-ms-wmp"},
 {"wmv", "video/x-ms-wmv"},
 {"wmx", "video/x-ms-wmx"},
 {"wmz", "application/x-ms-wmz"},
 {"woff", "application/font-woff"},
 {"woff2", "application/font-woff2"},
 {"wpl", "application/vnd.ms-wpl"},
 {"wps", "application/vnd.ms-works"},
 {"wri", "application/x-mswrite"},
 {"wrl", "x-world/x-vrml"},
 {"wrz", "x-world/x-vrml"},
 {"wsc", "text/scriptlet"},
 {"wsdl", "text/xml"},
 {"wvx", "video/x-ms-wvx"},
 {"x", "application/directx"},
 {"xaf", "x-world/x-vrml"},
 {"xaml", "application/xaml+xml"},
 {"xap", "application/x-silverlight-app"},
 {"xbap", "application/x-ms-xbap"},
 {"xbm", "image/x-xbitmap"},
 {"xdr", "text/plain"},
 {"xht", "application/xhtml+xml"},
 {"xhtml", "application/xhtml+xml"},
 {"xla", "application/vnd.ms-excel"},
 {"xlam", "application/vnd.ms-excel.addin.macroEnabled.12"},
 {"xlc", "application/vnd.ms-excel"},
 {"xld", "application/vnd.ms-excel"},
 {"xlk", "application/vnd.ms-excel"},
 {"xll", "application/vnd.ms-excel"},
 {"xlm", "application/vnd.ms-excel"},
 {"xls", "application/vnd.ms-excel"},
 {"xlsb", "application/vnd.ms-excel.sheet.binary.macroEnabled.12"},
 {"xlsm", "application/vnd.ms-excel.sheet.macroEnabled.12"},
 {"xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
 {"xlt", "application/vnd.ms-excel"},
 {"xltm", "application/vnd.ms-excel.template.macroEnabled.12"},
 {"xltx", "application/vnd.openxmlformats-officedocument.spreadsheetml.template"},
 {"xlw", "application/vnd.ms-excel"},
 {"xml", "text/xml"},
 {"xmp", "application/octet-stream"},
 {"xmta", "application/xml"},
 {"xof", "x-world/x-vrml"},
 {"xpm", "image/x-xpixmap"},
 {"xps", "application/vnd.ms-xpsdocument"},
 {"xrm-ms", "text/xml"},
 {"xsc", "application/xml"},
 {"xsd", "text/xml"},
 {"xsf", "text/xml"},
 {"xsl", "text/xml"},
 {"xslt", "text/xml"},
 {"xsn", "application/octet-stream"},
 {"xspf", "application/xspf+xml"},
 {"xss", "application/xml"},
 {"xtp", "application/octet-stream"},
 {"xwd", "image/x-xwindowdump"},
 {"yaml", "application/yaml"},
 {"yml", "application/yaml"},
 {"z", "application/x-compress"},
 {"zip", "application/zip"},
};

static int32_t
MimeStrCmp(wchar_t *W, char *S)
{
 while (*W && ((char)*W == *S))
 {
  W++;
  S++;
 }
 return (char)*W - *S;
}

// Str should point to . character
// https://research.google/blog/extra-extra-read-all-about-it-nearly-all-binary-searches-and-mergesorts-are-broken/
static char *
MimeLookupExtnW(wchar_t *Str)
{
 char *Ret = 0;
 if (*Str++ == '.')
 {
   uint32_t L = 0;
   uint32_t R = HttpArrLn(MimeTab) - 1;
   while (L <= R)
   {
    uint32_t M = L + ((R - L) / 2);
    int32_t Res = MimeStrCmp(Str, (char *)MimeTab[M][0]);
    if (Res > 0)
    {
     L = M + 1;
    }
    else if (Res < 0)
    {
     R = M - 1;
    }
    else
    {
     Ret = (char *)MimeTab[M][1];
     break;
    }
   }
 }
 return Ret;
}

// Ln must include null char
static char *
MimeLookupPathW(wchar_t *Path, size_t Ln)
{
 char *Ret = 0;
 wchar_t *Extn = 0;
 HRESULT Res = PathCchFindExtension(Path, Ln, &Extn);
 if (Res == S_OK)
 {
  Ret = MimeLookupExtnW(Extn);
 }

 if (!Ret)
 {
  Ret = "application/octet-stream";
 }
 return Ret;
}



// HTTP



#define HttpStrLn(Str) (sizeof(Str) / sizeof(Str[0]) - 1)

#define Http100 "100"
#define Http101 "101"
#define Http102 "102"
#define Http200 "200"
#define Http201 "201"
#define Http202 "202"
#define Http203 "203"
#define Http204 "204"
#define Http205 "205"
#define Http206 "206"
#define Http207 "207"
#define Http208 "208"
#define Http226 "226"
#define Http300 "300"
#define Http301 "301"
#define Http302 "302"
#define Http303 "303"
#define Http304 "304"
#define Http305 "305"
#define Http307 "307"
#define Http308 "308"
#define Http400 "400"
#define Http401 "401"
#define Http402 "402"
#define Http403 "403"
#define Http404 "404"
#define Http405 "405"
#define Http406 "406"
#define Http407 "407"
#define Http408 "408"
#define Http409 "409"
#define Http410 "410"
#define Http411 "411"
#define Http412 "412"
#define Http413 "413"
#define Http414 "414"
#define Http415 "415"
#define Http416 "416"
#define Http417 "417"
#define Http418 "418"
#define Http421 "421"
#define Http422 "422"
#define Http423 "423"
#define Http424 "424"
#define Http425 "425"
#define Http426 "426"
#define Http428 "428"
#define Http429 "429"
#define Http431 "431"
#define Http451 "451"
#define Http500 "500"
#define Http501 "501"
#define Http502 "502"
#define Http503 "503"
#define Http504 "504"
#define Http505 "505"
#define Http506 "506"
#define Http507 "507"
#define Http508 "508"
#define Http510 "510"
#define Http511 "511"

typedef uint32_t http_method_kind;
enum
{
 HttpMethodGet,
 HttpMethodPost,
 HttpMethodPatch,
 HttpMethodPut,
 HttpMethodDelete,
};

typedef uint32_t http_linebreak_style;
enum
{
 HttpLinebreakUndefined,
 HttpLinebreakCrlf,
 HttpLinebreakLf,
};

// host header value
typedef uint32_t http_host_kind;
enum
{
 HttpHostInvalid,
 HttpHostIpv6,
 HttpHostIpv4,
 HttpHostDomain,
};

// connection header value
typedef uint32_t http_connection_kind;
enum
{
 HttpConnectionUndefined,
 HttpConnectionClose,
 HttpConnectionKeepAlive,
};

typedef struct http_parse_ctx http_parse_ctx;
struct http_parse_ctx
{
 http_method_kind MethodType;
 char Path[MAX_PATH];
 uint16_t PathLn;
 http_linebreak_style LinebreakStyle;

 // headers

 http_host_kind HostKind;
 union
 {
  uint32_t HostIpv4;
  struct
  {
   char HostDomainName[255];
   size_t HostDomainNameLn;
  };
 };
 uint16_t HostPort;
 http_connection_kind ConnectionKind;
};


// typedef struct http_fpath http_fpath;
// struct http_fpath
// {
//  wchar_t Path[MAX_PATH];
//  uint32_t Ln;
// };

// static http_fpath
// HttpResolveFpathFromWStr(wchar_t *Path)
// {
//  http_fpath Ret = {0};
//  Ret.Ln = GetFullPathNameW(Path, MAX_PATH, Ret.Path, 0);
//  return Ret;
// }

// static void
// HttpResolveReqFpath(http_ctx *Ctx, http_fpath *BaseDir, http_fpath *Out)
// {
//  wchar_t CatStr[MAX_PATH] = {0};
//  wcscat_s(CatStr, MAX_PATH, BaseDir->Path);
//  wcsncat_s(CatStr, MAX_PATH, Ctx->Req->CookedUrl.pAbsPath, Ctx->Req->CookedUrl.AbsPathLength);

//  Out->Ln = GetFullPathNameW(CatStr, MAX_PATH, Out->Path, 0);
//  if (Out->Ln < BaseDir->Ln)
//  {
//   *Out = (http_fpath){0};
//  }
// }



// Helpers



static void
HttpMemcpyAdvance(uint32_t *Run, void **Dst, size_t *DstLn, void *Src, size_t SrcLn)
{
 if (*Run)
 {
  *Run = *DstLn >= SrcLn;
  if (*Run)
  {
   memcpy_s(*Dst, *DstLn, Src, SrcLn);
   *(char **)Dst += SrcLn;
   *DstLn -= SrcLn;
  }
 }
}



// Strings



static uint32_t
ChrIsAlpha(char C)
{
 C |= 0x20;
 return C >= 'a' && C <= 'z';
}

static uint32_t
ChrIsDigit(char C)
{
 return C >= '0' && C <= '9';
}

static uint32_t
ChrIsAlphaNum(char C)
{
 return ChrIsAlpha(C) || ChrIsDigit(C);
}

// from abnf spec:
// https://www.rfc-editor.org/rfc/rfc5234
static uint32_t
ChrIsVChar(char C)
{
 return (uint8_t)C >= 0x21 && (uint8_t)C <= 0x7E;
}

static uint32_t
ChrIsSubdelim(char C)
{
 return C == '!' || C == '$' || C == '&' || C == '\'' || C == '(' || C == ')' || C == '*' || C == '+' || C == ',' || C == ';' || C == '=';
}

// https://datatracker.ietf.org/doc/html/rfc9110#name-tokens
static uint32_t
ChrIsTChar(char C)
{
 return C == '!' || C == '#' || C == '$' || C == '%' || C == '&' || C == '\'' || C == '*'
   || C == '+' || C == '-' || C == '.' || C == '^' || C == '_' || C == '`' || C == '|' || C == '~' || ChrIsAlphaNum(C);
}

// https://datatracker.ietf.org/doc/html/rfc9110#name-field-values
static uint32_t
ChrIsHttpFieldContent(char C)
{
 return ChrIsVChar(C) || C == ' ' || C == '\t';
}

// https://datatracker.ietf.org/doc/html/rfc3986#section-2.3
static uint32_t
ChrIsUriUnreserved(char C)
{
 return ChrIsAlphaNum(C) || (C == '-') || (C == '.') || (C == '_') || (C == '~');
}

static uint32_t
ChrIsHttpPathAcceptable(char C)
{
 // Subdelims from spec minus some of the ones that windows doesn't allow for filenames
 // https://datatracker.ietf.org/doc/html/rfc3986#section-2.2
 // https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file#naming-conventions
 uint32_t IsWindowsCompatibleSubDelim = (C == '!') || (C == '$') || (C == '&') || (C == '\'') || (C == '(') || (C == ')') || (C == '+') || (C == ',') || (C == ';') || (C == '=');

 // path-absolute from:
 // https://datatracker.ietf.org/doc/html/rfc3986#section-3.3
 return (C == '/') || ChrIsUriUnreserved(C) || IsWindowsCompatibleSubDelim || (C == '@');
}

static uint32_t
ChrIsHex(char C)
{
 char A = C | 0x20;
 return ChrIsDigit(C) || (A >= 'a' && A <= 'f');
}

static uint32_t
ChrHexDecode(char C, char *Out)
{
 char A = C | 0x20;
 if (ChrIsDigit(C))
 {
  *Out = C - '0';
  return 1;
 }
 else if (A >= 'a' && A <= 'f')
 {
  *Out = A - 'a' + 10;
  return 1;
 }
 return 0;
}

static char
ChrToLower(char C)
{
 return ChrIsAlpha(C) ? C | 0x20 : C;
}

static uint32_t
StrEq(char *Str1, size_t Str1Ln, char *Str2, size_t Str2Ln)
{
 return (Str1Ln == Str2Ln) && !memcmp(Str1, Str2, Str1Ln);
}

static uint32_t
StrStartsWith(char *Str, size_t StrLn, char *Prefix, size_t PrefixLn)
{
 return StrLn >= PrefixLn ? !memcmp(Str, Prefix, PrefixLn) : 0;
}

// case insensitive
static uint32_t
StrStartsWithI(char *Str, size_t StrLn, char *Prefix, size_t PrefixLn)
{
 return StrLn >= PrefixLn ? !_strnicmp(Str, Prefix, PrefixLn) : 0;
}

static uint32_t
ViewCmpShift(char **View, char *ViewEnd, char *Prefix, size_t PrefixLn)
{
 if (*View < ViewEnd)
 {
  size_t StrLn = ViewEnd - *View;
  if (StrStartsWith(*View, StrLn, Prefix, PrefixLn))
  {
   *View += PrefixLn;
   return 1;
  }
 }
 return 0;
}

// case insensitive
static uint32_t
ViewCmpShiftI(char **View, char *ViewEnd, char *Prefix, size_t PrefixLn)
{
 if (*View < ViewEnd)
 {
  size_t StrLn = ViewEnd - *View;
  if (StrStartsWithI(*View, StrLn, Prefix, PrefixLn))
  {
   *View += PrefixLn;
   return 1;
  }
 }
 return 0;
}

// optional whitespace:
// https://datatracker.ietf.org/doc/html/rfc9110#appendix-A-2
static uint32_t
ViewCmpShiftOWS(char **View, char *ViewEnd)
{
 uint32_t Ret = 0;
 while ((*View < ViewEnd) && ((**View == ' ') || (**View == '\t')))
 {
  Ret = 1;
  (*View)++;
 }
 return Ret;
}

// check for /. and /..
static uint32_t
HttpIsRelativePathSegment(char *Str, size_t StrLn)
{
 char *Dot = "/.";
 char *DotDot = "/..";
 if (StrEq(Dot, strlen(Dot), Str, StrLn) || StrEq(DotDot, strlen(DotDot), Str, StrLn))
 {
  return 1;
 }
 return 0;
}

// check these as well once utf8 support added
// "COM¹",
// "COM²",
// "COM³",
// "LPT¹",
// "LPT²",
// "LPT³",
// windows doesn't allow certain filenames...
static uint32_t
HttpIsReservedPathSegment(char *Str, size_t StrLn)
{
 char *ReservedNames[] = {
  "CON",
  "PRN",
  "AUX",
  "NUL",
  "COM1",
  "COM2",
  "COM3",
  "COM4",
  "COM5",
  "COM6",
  "COM7",
  "COM8",
  "COM9",
  "LPT1",
  "LPT2",
  "LPT3",
  "LPT4",
  "LPT5",
  "LPT6",
  "LPT7",
  "LPT8",
  "LPT9",
 };

 for (size_t I = 0; I < HttpArrLn(ReservedNames); ++I)
 {
  char *Name = ReservedNames[I];
  if (StrEq(Name, strlen(Name), Str, StrLn))
  {
   return 1;
  }
 }
 return 0;
}

static uint32_t
HttpSegmentIter(char **View, char *ViewEnd)
{
 if ((*View >= ViewEnd) || (**View != '/'))
 {
  return 0;
 }
 while (*View < ViewEnd)
 {
  (*View)++;
  char C = **View;
  if (C == '/')
  {
   return 1;
  }
 }
 return 1;
}

static char *
HttpFindNextSpace(char *Start, char *End)
{
 while (Start < End)
 {
  if (*Start == ' ')
  {
   return Start;
  }
  Start++;
 }
 return 0;
}

static uint32_t
HttpPctDecode(char *S, size_t StrLn, char *Out)
{
 if ((*S == '%') && (StrLn >= 3))
 {
  char Hex1 = 0;
  char Hex2 = 0;
  if (ChrHexDecode(S[1], &Hex1) && ChrHexDecode(S[2], &Hex2))
  {
   *Out = (Hex1 * 16) + Hex2;
   return 1;
  }
 }
 return 0;
}

static uint32_t
HttpEvaluateSegment(char *Start, char *End, char **Out, char *OutEnd)
{
 size_t Ln = End - Start;
 if (HttpIsRelativePathSegment(Start, Ln) || HttpIsReservedPathSegment(Start, Ln))
 {
  return 0;
 }

 char *C = Start;
 while ((C < End) && (*Out < OutEnd))
 {
  if (ChrIsHttpPathAcceptable(*C))
  {
   **Out = *C;
   (*Out)++;
   C++;
  }
  else if (*C == '%')
  {
   // only allow certain pct decodings
   if (HttpPctDecode(C, End - C, *Out) && **Out == ' ')
   {
     (*Out)++;
     C += 3;
   }
   else
   {
    // faulty pct encoding
    return 0;
   }
  }
  else
  {
   return 0;
  }
 }
 
 return 1;
}

static uint32_t
HttpGetPath(char **View, char *ViewEnd, char *Out, size_t OutLn)
{
 if ((*View >= ViewEnd) || (**View != '/'))
 {
  return 0;
 }
 char *PathEnd = HttpFindNextSpace(*View, ViewEnd);
 if (!PathEnd)
 {
  return 0;
 }

 char *O = Out;
 char *OutEnd = O + OutLn;
 char *LastSlash = *View;
 while (HttpSegmentIter(View, PathEnd))
 {
  if (LastSlash != *View)
  {
   if (HttpEvaluateSegment(LastSlash, *View, &O, OutEnd))
   {
   }
   else
   {
    // err
    return 0;
   }
  }
  LastSlash = *View;
 }

 // go one past space, there should be a version str here
 *View = PathEnd + 1;
 if (*View < ViewEnd)
 {
  return (uint32_t)(O - Out);
 }
 else
 {
  return 0;
 }
}

static uint32_t
HttpGetLinebreak(char **View, char *ViewEnd, http_parse_ctx *Ctx)
{
 if (Ctx->LinebreakStyle == HttpLinebreakCrlf)
 {
  char *LinebreakStr = "\r\n";
  return ViewCmpShift(View, ViewEnd, LinebreakStr, strlen(LinebreakStr));
 }
 if (Ctx->LinebreakStyle == HttpLinebreakLf)
 {
  char *LinebreakStr = "\n";
  return ViewCmpShift(View, ViewEnd, LinebreakStr, strlen(LinebreakStr));
 }
 return 0;
}

static uint32_t
HttpGetU32(char **View, char *ViewEnd, uint32_t *Out)
{
 if (*View < ViewEnd)
 {
  size_t StrLn = ViewEnd - *View;
  char *End = 0;
  atoi_result Res = 0;
  uint32_t N = atoi_u32_yy(*View, StrLn, &End, &Res);
  if (Res == atoi_result_suc)
  {
   *View = End;
   *Out = N;
   return 1;
  }
 }
 return 0;
}

static uint32_t
HttpGetIpv4(char **View, char *ViewEnd, uint32_t *Out)
{
 if (*View < ViewEnd)
 {
  char *Dot = ".";

  uint32_t N1 = 0;
  if (!HttpGetU32(View, ViewEnd, &N1) || (N1 > 255)) return 0;
  if (!ViewCmpShift(View, ViewEnd, Dot, strlen(Dot))) return 0;
  uint32_t N2 = 0;
  if (!HttpGetU32(View, ViewEnd, &N2) || (N2 > 255)) return 0;
  if (!ViewCmpShift(View, ViewEnd, Dot, strlen(Dot))) return 0;
  uint32_t N3 = 0;
  if (!HttpGetU32(View, ViewEnd, &N3) || (N3 > 255)) return 0;
  if (!ViewCmpShift(View, ViewEnd, Dot, strlen(Dot))) return 0;
  uint32_t N4 = 0;
  if (!HttpGetU32(View, ViewEnd, &N4) || (N4 > 255)) return 0;

  *Out = N1 | (N2 << 8) | (N3 << 16) | (N4 << 24);
  return 1;
 }
 return 0;
}

// todo 
static uint32_t
HttpGetIpv6(char **View, char *ViewEnd)
{
 return 0;
}

// todo idna2008 punycode support
// return bytes written to Out
static size_t
HttpGetDomainName(char **View, char *ViewEnd, char *Out, size_t OutLn)
{
 size_t WriteLn = 0;
 // max dns name 253 chars, excluding root domain '.' and null terminator
 uint32_t Fuel = 253;
 // each name can only be a 63 chars long
 uint32_t DotFuel = 64;

 // need to keep track of last seen chr since the name could end in '.'
 char LastChr = 0;
 uint32_t JustSawDot = 0;
 while (*View < ViewEnd && WriteLn < OutLn && Fuel-- && DotFuel--)
 {
  char C = **View;
  if ((ChrIsAlphaNum(C) || C == '-'))
  {
   if (!DotFuel)
   {
    return 0;
   }
   Out[WriteLn++] = ChrToLower(C);
   (*View)++;
   LastChr = C;
   JustSawDot = 0;
  }
  else if (C == '.')
  {
   if (JustSawDot)
   {
    return 0;
   }
   Out[WriteLn++] = C;
   (*View)++;
   DotFuel = 64;
   JustSawDot = 1;
  }
  else
  {
   break;
  }
 }

 // get rid of root domain
 if (WriteLn && Out[WriteLn - 1] == '.')
 {
  Out[--WriteLn] = 0;
 }

 if (WriteLn && ChrIsAlpha(Out[0]) && ChrIsAlphaNum(LastChr))
 {
  return WriteLn;
 }
 return 0;
}

// host = IP-literal / IPv4address / reg-name
// reg-name = *( unreserved / pct-encoded / sub-delims )
// sub-delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
static uint32_t
HttpGetHost(char **View, char *ViewEnd, http_parse_ctx *Ctx)
{
 if (HttpGetIpv6(View, ViewEnd))
 {
  Ctx->HostKind = HttpHostIpv6;
  return 1;
 }
 else if (HttpGetIpv4(View, ViewEnd, &Ctx->HostIpv4))
 {
  Ctx->HostKind = HttpHostIpv4;
  return 1;
 }
 else
 {
  Ctx->HostDomainNameLn = HttpGetDomainName(View, ViewEnd, Ctx->HostDomainName, sizeof(Ctx->HostDomainName));
  if (Ctx->HostDomainNameLn)
  {
   Ctx->HostKind = HttpHostDomain;
   return 1;
  }
  return 0;
 }
 return 0;
}

static uint32_t
HttpGetPort(char **View, char *ViewEnd, http_parse_ctx *Ctx)
{
 if (*View < ViewEnd)
 {
  if (**View == ':')
  {
   (*View)++;
   uint32_t Port = 0;
   if (HttpGetU32(View, ViewEnd, &Port))
   {
    Ctx->HostPort = Port;
    return 1;
   }
  }
 }
 return 0;
}

static uint32_t
HttpGetArbitraryHeader(char **View, char *ViewEnd, http_parse_ctx *Ctx)
{
 uint32_t GotHeaderName = 0;
 uint32_t AtLeastOneTchar = 0;
 while (*View < ViewEnd)
 {
  uint8_t C = **View;
  uint32_t IsTchar = ChrIsTChar(C);
  if (IsTchar)
  {
   (*View)++;
   AtLeastOneTchar = 1;
  }
  else if (C == ':' && AtLeastOneTchar)
  {
   (*View)++;
   GotHeaderName = 1;
   break;
  }
  else
  {
   GotHeaderName = 0;
   break;
  }
 }

 if (GotHeaderName)
 {
  uint32_t AtLeastOneFieldContent = 0;
  while (*View < ViewEnd)
  {
   if (ChrIsHttpFieldContent(**View))
   {
    AtLeastOneFieldContent = 1;
    (*View)++;
   }
   else if (AtLeastOneFieldContent && HttpGetLinebreak(View, ViewEnd, Ctx))
   {
    return 1;
   }
   else
   {
    break;
   }
  }
 }
 return 0;
}

static uint32_t
HttpGetHeader(char **View, char *ViewEnd, http_parse_ctx *Ctx)
{
 char *ConnectionStr = "connection:";
 if (ViewCmpShiftI(View, ViewEnd, ConnectionStr, strlen(ConnectionStr)))
 {
  ViewCmpShiftOWS(View, ViewEnd);

  char *CloseStr = "close";
  char *KeepAliveStr = "keep-alive";
  if (ViewCmpShiftI(View, ViewEnd, CloseStr, strlen(CloseStr)))
  {
   Ctx->ConnectionKind = HttpConnectionClose;
   return 1;
  }
  else if (ViewCmpShiftI(View, ViewEnd, KeepAliveStr, strlen(KeepAliveStr)))
  {
   Ctx->ConnectionKind = HttpConnectionKeepAlive;
   return 1;
  }
  return 0;
 }

 char *HostStr = "host:";
 if (ViewCmpShiftI(View, ViewEnd, HostStr, strlen(HostStr)))
 {
  ViewCmpShiftOWS(View, ViewEnd);

  if (HttpGetHost(View, ViewEnd, Ctx))
  {
   HttpGetPort(View, ViewEnd, Ctx);
   if (HttpGetLinebreak(View, ViewEnd, Ctx))
   {
    return 1;
   }
  }
 }

 if (HttpGetArbitraryHeader(View, ViewEnd, Ctx))
 {
  return 1;
 }
 
 return 0;
}



// API



static uint32_t
HttpParseRequest(char *Arr, size_t ArrLn, http_parse_ctx *Ctx)
{
 char *View = Arr;
 char *ViewEnd = Arr + ArrLn;

 char *HttpGetStr = "GET ";
 char *Version = "HTTP/1.1";
 char *Lf = "\n";
 char *Crlf = "\r\n";

 // parse first line
 if (ViewCmpShift(&View, ViewEnd, HttpGetStr, strlen(HttpGetStr)))
 {
  Ctx->MethodType = HttpMethodGet;
  Ctx->PathLn = HttpGetPath(&View, ViewEnd, Ctx->Path, sizeof(Ctx->Path));
  if (Ctx->PathLn)
  {
   if (ViewCmpShift(&View, ViewEnd, Version, strlen(Version)))
   {
    if (ViewCmpShift(&View, ViewEnd, Lf, strlen(Lf)))
    {
     Ctx->LinebreakStyle = HttpLinebreakLf;
    }
    else if (ViewCmpShift(&View, ViewEnd, Crlf, strlen(Crlf)))
    {
     Ctx->LinebreakStyle = HttpLinebreakCrlf;
    }
   }
  }
 }

 uint32_t HeaderEndFound = 0;
 if (Ctx->LinebreakStyle != HttpLinebreakUndefined)
 {
  while (View < ViewEnd)
  {
   if (HttpGetLinebreak(&View, ViewEnd, Ctx))
   {
    HeaderEndFound = 1;
    break;
   }
   if (!HttpGetHeader(&View, ViewEnd, Ctx))
   {
    return 0;
   }
  }
 }

 // todo "parse" body
 if (HeaderEndFound)
 {
  return 1;
 }

 return 0;
}

static size_t
HttpCreateResponse(char * Status, char *MimeType, uint32_t MimeTypeLn, char *Body, size_t BodyLn, char *Out, size_t OutLn)
{
 char *FirstLine = "HTTP/1.1 ";
 char *NewLine = "\r\n";
 char *ContentType = "Content-Type: ";
 char *ConnectionClose = "Connection: close";
 uint32_t Run = 1;
 void *OutView = Out;
 size_t OutViewLn = OutLn;
 HttpMemcpyAdvance(&Run, &OutView, &OutViewLn, FirstLine, strlen(FirstLine));
 HttpMemcpyAdvance(&Run, &OutView, &OutViewLn, Status, 3);
 HttpMemcpyAdvance(&Run, &OutView, &OutViewLn, NewLine, strlen(NewLine));
 HttpMemcpyAdvance(&Run, &OutView, &OutViewLn, ContentType, strlen(ContentType));
 HttpMemcpyAdvance(&Run, &OutView, &OutViewLn, MimeType, MimeTypeLn);
 HttpMemcpyAdvance(&Run, &OutView, &OutViewLn, NewLine, strlen(NewLine));
 HttpMemcpyAdvance(&Run, &OutView, &OutViewLn, ConnectionClose, strlen(ConnectionClose));
 HttpMemcpyAdvance(&Run, &OutView, &OutViewLn, NewLine, strlen(NewLine));
 HttpMemcpyAdvance(&Run, &OutView, &OutViewLn, NewLine, strlen(NewLine));
 HttpMemcpyAdvance(&Run, &OutView, &OutViewLn, Body, BodyLn);
 if (Run)
 {
  return (char *)OutView - Out;
 }
 else
 {
  return 0;
 }
}
