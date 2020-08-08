#pragma once
#include "common/patching/FunHook.h"
#include "RFGR_Types/rfg/Xml.h"
#include "RFGR_Types/rfg/stream2/cfile.h"
#include "RFGR_Types/rfg/keen/FileSystem.h"

//FunHook<xml_element* (const char* filename, void* dest)> XmlParseHook
//{
//    0x0,
//    [](const char* filename, void* dest) -> xml_element*
//    {
//        if (strcmp(filename, "weapons.xtbl") == 0)
//        {
//            printf("Found weapons.xtbl xml_parse call!\n");
//            filename = "C:/Users/moneyl/source/repos/RSL2/weapons.xtbl";
//        }
//        auto a = 2;
//        printf("xml_parse called with \"%s\"\n", filename);
//        xml_element* result = XmlParseHook.CallTarget(filename, dest);
//        auto b = 2;
//        return result;
//    }
//};
//
//FunHook<cfile* (char* buf, u32 buf_size, const char* open_mode, vlib_platform disk_platform)> CfOpenHook0
//{
//    0x0,
//    [](char* buf, u32 buf_size, const char* open_mode, vlib_platform disk_platform) -> cfile*
//    {
//        string filename(buf, buf_size);
//        printf("CfOpen0 called with: \"%s\"\n", filename.c_str());
//        
//        auto a = 2;
//        cfile* result = CfOpenHook0.CallTarget(buf, buf_size, open_mode, disk_platform);
//        auto b = 2;
//        return result;
//    }
//};
//
//FunHook<cfile* (const char* filename_in, const char* open_mode, vlib_platform disk_platform, bool async)> CfOpenHook1
//{
//    0x0,
//    [](const char* filename_in, const char* open_mode, vlib_platform disk_platform, bool async) -> cfile*
//    {
//        printf("CfOpen0 called with: \"%s\"\n", filename_in);
//
//        auto a = 2;
//        cfile* result = CfOpenHook1.CallTarget(filename_in, open_mode, disk_platform, async);
//        auto b = 2;
//        return result;
//    }
//};
//
////cf_keen_open_file *__cdecl keen_cf_open_file(const char *filename, const char *mode, cf_io_media_types media_type, bool async) //0x001B6790
//FunHook<cf_keen_open_file* (const char* filename, const char* mode, cf_io_media_types media_type, bool async)> KeenCfOpenFileHook
//{
//    0x0,
//    [](const char* filename, const char* mode, cf_io_media_types media_type, bool async) -> cf_keen_open_file*
//    {
//        printf("keen_cf_open_file(\"%s\", \"%s\", %d, %d)\n", filename, mode, media_type, async);
//
//        auto a = 2;
//        cf_keen_open_file* result = KeenCfOpenFileHook.CallTarget(filename, mode, media_type, async);
//        auto b = 2;
//        return result;
//    }
//};