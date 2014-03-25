#pragma once


#include "CommandSyntax.h"
#include "CommandLexer.h"
#include "ScriptCommand.h"
#include "ScriptParameter.h"
#include "ScriptToken.h"
#include <algorithm>


namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         class CommandTree;

         /// <summary>Script error</summary>
         class LogicExport ErrorToken : public TokenBase
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
#ifdef LOGIC_COMPILER_FIX
            ErrorToken() : Line(0) 
               { throw AlgorithmException(HERE, L"LogicLibrary compiler-fix code executed"); }
#endif
            /// <summary>Create error for token</summary>
            ErrorToken(const GuiString& msg, UINT line, const ScriptToken& tok) 
               : TokenBase(tok), Message(msg), Text(tok.Text), Line(line)
            {}
            /// <summary>Create error for a character range</summary>
            ErrorToken(const GuiString& msg, UINT line, const GuiString& txt, const CHARRANGE& r) 
               : TokenBase(r.cpMin, r.cpMax), Message(msg), Text(txt), Line(line)
            {}
            /// <summary>Create error for a character range</summary>
            ErrorToken(const GuiString& msg, UINT line, const GuiString& txt, UINT start, UINT end) 
               : TokenBase(start,end), Message(msg), Text(txt), Line(line)
            {}

            // ---------------------- ACCESSORS ------------------------	

            // ----------------------- MUTATORS ------------------------
#ifdef LOGIC_COMPILER_FIX
         public:
            ErrorToken& operator=(const ErrorToken& r) const
               { throw AlgorithmException(HERE, L"LogicLibrary compiler-fix code executed"); }
#endif
            // -------------------- REPRESENTATION ---------------------
         public:
            const GuiString Message,   // Error message
                            Text;      // Erroneous token/line text
            const UINT      Line;      // 1-base line number
         };

         /// <summary>Vector of error tokens</summary>
         class LogicExport ErrorArray : public vector<ErrorToken> 
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            /// <summary>Create empty array</summary>
            ErrorArray()
            {}
            /// <summary>Create array from single token</summary>
            ErrorArray(const ErrorToken& e) {
               push_back(e);
            }

            // ---------------------- ACCESSORS ------------------------	

            // ----------------------- MUTATORS ------------------------
         public:
            /// <summary>Add an error to the array</summary>
            ErrorArray& operator+=(const ErrorToken& t)
            {
               push_back(t);
               return *this;
            }

            // -------------------- REPRESENTATION ---------------------

         };

         /// <summary>Shared pointer to a parse tree node</summary>
         class LogicExport CommandNodePtr : public shared_ptr<CommandTree> 
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            CommandNodePtr() : shared_ptr<CommandTree>(nullptr)
            {}
            CommandNodePtr(CommandTree* node) : shared_ptr<CommandTree>(node)
            {}

            // ---------------------- ACCESSORS ------------------------	

            // ----------------------- MUTATORS ------------------------

            // -------------------- REPRESENTATION ---------------------
         };

         /// <summary>Vector of parse tree node pointers</summary>
         typedef vector<CommandNodePtr>   CommandNodeArray;

         /// <summary>Types of symbol available</summary>
         enum class SymbolType { Variable, Label };

         /// <summary>Represents a symbol used during refactoring process</summary>
         class LogicExport Symbol
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            /// <summary>Create symbol match.</summary>
            /// <param name="name">name without operators.</param>
            /// <param name="t">type.</param>
            /// <param name="line">1-based line number</param>
            /// <param name="txt">line text</param>
            /// <param name="comment">Whether line is commented</param>
            Symbol(const ScriptToken& tok, SymbolType t, UINT line, const wstring& txt, bool comment) 
               : Token(tok), Type(t), LineNumber(line), LineText(txt), Commented(comment)
            {}

            // ---------------------- ACCESSORS ------------------------	

            // ----------------------- MUTATORS ------------------------

            // -------------------- REPRESENTATION ---------------------
         public:
            const SymbolType  Type;          // Type
            const ScriptToken Token;         // Name
            const UINT        LineNumber;    // 1-based line number
            const GuiString   LineText;      // Line text
            const bool        Commented;     // Whether commented
         };

         /// <summary></summary>
         typedef list<Symbol>  SymbolList;
            
         /// <summary>Represents a script command and its descendants, if any</summary>
         class LogicExport CommandTree 
         {
            // ------------------------ TYPES --------------------------
         protected:
            /// <summary>CommandTree array iterator</summary>
            typedef CommandNodeArray::const_iterator   NodeIterator;

            /// <summary>CommandTree predicate</summary>
            typedef function<bool (const CommandNodePtr&)>  NodeDelegate;

            /// <summary>Distinguishes tree state when printed to the console</summary>
            enum class InputState { Raw, Verified, Compiled };

            // --------------------- CONSTRUCTION ----------------------
         public:
            CommandTree();
            CommandTree(CommandTree* parent, const CommandTree* target);
            CommandTree(Conditional cnd, CommandSyntaxRef syntax, ParameterArray& params, const CommandLexer& lex, UINT line, bool commented);
            CommandTree(Conditional cnd, CommandSyntaxRef syntax, ParameterArray& infix, ParameterArray& postfix, const CommandLexer& lex, UINT line, bool commented);
            ~CommandTree();

            // ------------------------ STATIC -------------------------
         protected:
            static NodeDelegate  isConditionalAlternate;
            static NodeDelegate  isConditionalEnd;
            static NodeDelegate  isExecutableCommand;
            static NodeDelegate  isStandardCommand;
            static NodeDelegate  isSkipIfCompatible;
            
            static const wchar*  GetString(InputState s);

#ifdef VALIDATION
            /// <summary>An invisible node that functions as a jump target with address 'script_length+1'</summary>
            static CommandTree  EndOfScript;
#endif
            // --------------------- PROPERTIES ------------------------
         public:
            PROPERTY_GET(BranchLogic,Logic,GetBranchLogic);
            PROPERTY_GET(GuiString,LineCode,GetLineCode);

            // ---------------------- ACCESSORS ------------------------		
         public:
            void          FindAll(const wstring& name, SymbolType type, SymbolList& results) const;
            BranchLogic   GetBranchLogic() const;
            GuiString     GetLineCode() const;
            bool          Is(UINT ID) const;
            bool          Is(CommandType t) const;
            void          Print(int depth = 0) const;
            void          ToList(CommandNodeArray& l) const;
               
         protected:
            CommandTree*  FindAncestor(BranchLogic l) const;
            NodeIterator  FindChild(const CommandTree* child) const;
            CommandTree*  FindConditionalAlternate() const;
            CommandTree*  FindConditionalEnd() const;
            CommandTree*  FindLabel(const wstring& name) const;
            CommandTree*  FindNextCommand() const;
            CommandTree*  FindNextSibling() const;
            CommandTree*  FindPrevSibling() const;
            CommandTree*  FindRoot() const;
            CommandTree*  FindSibling(NodeDelegate d, const wchar* help) const;
            CommandTree*  GetLastExecutableChild() const;
            wstring       GetScriptCallName() const;
            bool          HasExecutableChild() const;
            bool          IsRoot() const;
            ErrorToken    MakeError(const GuiString& msg) const;
            ErrorToken    MakeError(const GuiString& msg, const ScriptToken& tok) const;
            void          VerifyLogic(ErrorArray& errors) const;
            void          VerifyParameter(const ScriptParameter& p, UINT index, const ScriptFile& script, ErrorArray& errors) const;
            void          VerifyTermination(ErrorArray& errors) const;
               
            // ----------------------- MUTATORS ------------------------
         public:
            CommandNodePtr Add(CommandNodePtr node);
            void           Compile(ScriptFile& script, ErrorArray& errors);
            void           Verify(ScriptFile& script, ErrorArray& errors);
            
         protected:
            void  FinalizeLinkage(ErrorArray& errors);
            void  GenerateCommands(ScriptFile& script, ErrorArray& errors);
            void  IdentifyConstants(ScriptFile& script, ErrorArray& errors);
            void  IdentifyVariables(ScriptFile& script, ErrorArray& errors);
            void  IndexCommands(UINT& next);
            void  InsertJump(NodeIterator pos, const CommandTree* target);
            void  LinkCommands(ErrorArray& errors);
            void  RevertCommandComment(CommandTree* child);
            void  ReplaceChild(CommandTree* oldChild, CommandTree* newChild);
            void  VerifyCommand(const ScriptFile& script, ErrorArray& errors);
            
            // -------------------- REPRESENTATION ---------------------
         public:
            CommandTree*       Parent;        // Parent node
            CommandNodeArray   Children;      // Child commands

            ParameterArray     Parameters,    // script parameters in display order
                               Postfix;       // expression parameters in postfix order
            CommandSyntaxRef   Syntax;        // command syntax
            const CommandTree* JumpTarget;    // Destination of unconditional-jmp or jump-if-false
            UINT               Index;         // 0-based standard codearray index
            bool               CmdComment;    // Whether a command comment  [false for ordinary comments]

            Conditional     Condition;        // Conditional
            const UINT      LineNumber;       // 1-based line number
            const CHARRANGE Extent;           // Start/end character offsets
            // Debug
            GuiString       LineText;         // Debug: line text
            InputState      State;            // Debug: processing state
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
