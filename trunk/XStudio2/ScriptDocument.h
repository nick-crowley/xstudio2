#pragma once
#include "Logic/ScriptFile.h"

/// <summary>Forward declaration</summary>
NAMESPACE_BEGIN2(GUI,Controls)
   class ScriptEdit;
NAMESPACE_END2(GUI,Controls)


/// <summary>Documents</summary>
NAMESPACE_BEGIN2(GUI,Documents)

   /// <summary>MSCI Script document template</summary>
   class ScriptDocTemplate : public CMultiDocTemplate
   {
      // --------------------- CONSTRUCTION ----------------------
   public:
      ScriptDocTemplate();

      // ------------------------ STATIC -------------------------

      DECLARE_DYNAMIC(ScriptDocTemplate)

      // ---------------------- ACCESSORS ------------------------	
      virtual Confidence MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch);
   };

   /// <summary>MSCI Script document</summary>
   class ScriptDocument : public CDocument
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------

   protected: // create from serialization only
	   ScriptDocument();
   public:
	   virtual ~ScriptDocument();

      // ------------------------ STATIC -------------------------

   protected:
      DECLARE_DYNCREATE(ScriptDocument)
	   DECLARE_MESSAGE_MAP()

      // --------------------- PROPERTIES ------------------------
			
      // ---------------------- ACCESSORS ------------------------			

      // ----------------------- MUTATORS ------------------------
   public:
   #ifdef _DEBUG
	   virtual void AssertValid() const;
	   virtual void Dump(CDumpContext& dc) const;
   #endif
      void  AttachEdit(ScriptEdit& edit);

	   virtual BOOL OnNewDocument();
      virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
      virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	   virtual void Serialize(CArchive& ar);
   
      // -------------------- REPRESENTATION ---------------------
   public:
      ScriptFile  Script;

   private:
      ScriptEdit* Edit;
   };


NAMESPACE_END2(GUI,Documents)
