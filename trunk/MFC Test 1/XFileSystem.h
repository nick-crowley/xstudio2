#pragma once
#include "Common.h"
#include "XCatalog.h"

//using namespace Library::FileSystem;

namespace Library
{
   namespace FileSystem
   {
      class XCatalog;
      class XFileSystem;
      

      enum class FileSource  { Catalog, Physical };

      class XFileInfo
      {
      public:
         XFileInfo(Path p);
         XFileInfo(XFileSystem& vfs, XCatalog& cat, Path subPath, DWORD pos);
         virtual ~XFileInfo();

         XFileSystem* FileSystem;
         XCatalog*    Catalog;
         Path         FullPath;
         DWORD        Offset;
         FileSource   Source;
      };


      class XFileSystem
      {
      private:
         class CatalogCollection : private list<XCatalog>
         {
            typedef list<XCatalog> Base;

         public:
            CatalogCollection() {};
            ~CatalogCollection() { Clear(); };  // Unlock all catalogs

            __declspec(property(get=GetCount)) DWORD Count;

            void      Add(XCatalog&& c)  { Base::push_front(std::move(c)); }
            void      Clear()            { Base::clear();                  }
            DWORD     GetCount()         { return Base::size();            }
            iterator  Begin()            { return Base::begin();           }
            iterator  End()              { return Base::end();             }
         };

         

         class UniquePath : public binary_function<XFileInfo, XFileInfo, bool>
         {
         public:
            bool operator()(XFileInfo a, XFileInfo b)
            {
               return a.FullPath < b.FullPath;
            }
         };
         
         class FileCollection : private set<XFileInfo, UniquePath>
         {
            typedef set<XFileInfo, UniquePath> Base;

         public:
            __declspec(property(get=GetCount)) DWORD Count;

            bool      Add(XFileInfo&& f)  { return Base::insert(std::move(f)).second;  }
            void      Clear()             { Base::clear();                             }
            DWORD     GetCount()          { return Base::size();                       }
         };

      public:
         XFileSystem(Path folder, GameVersion version);
         virtual ~XFileSystem();

         DWORD  Enumerate();

         Path         GetFolder()   { return Folder;  }
         GameVersion  GetVersion()  { return Version; }

      private:
         DWORD  EnumerateCatalogs();
         DWORD  EnumerateFiles();

         CatalogCollection  Catalogs;
         FileCollection     Files;
         Path               Folder;
         GameVersion        Version;
      };


      
   }
}
