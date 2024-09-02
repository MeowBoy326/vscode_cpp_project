#ifndef _Scanner_H_
#define _Scanner_H_

#include <string>
#include <fstream>
#include <deque>
#include <set>

using namespace std;

class Scanner
{
private:
    enum State	// ת��ͼ�е�״̬
    {
		STATE_UNREADY = -1,
        STATE_START,		// ��ʼ״̬
        STATE_ID,			// ��ʶ��״̬
        STATE_INT,			// ������״̬
        STATE_CHAR,			// �ַ�״̬		
        STATE_CHAR_A,
        STATE_CHAR_B,
        STATE_CHAR_C,
        STATE_FLOAT,		// ������״̬
        STATE_D_FLOAT,		// �ӽ���С����ĸ�����״̬
        STATE_E_FLOAT,		// �ӽ���ѧ�������ĸ�����״̬
        STATE_STRING,		// �ַ���״̬
        STATE_S_STRING,		// ����ת���ַ����ַ���
        STATE_SYMBOL, 
        STATE_COMMENT,   	// ע��״̬
        STATE_P_COMMENT,	// ��Ҫ����ע��״̬
        STATE_DONE,			// ����״̬
        STATE_ERROR			// ����״̬
    };

public:
    set<string> m_vtkeyWords;
    set<string> m_vtStrSymbols;
    enum TokenType
    {      
        KEY_WORD,
        ID,				// ��ʶ��
        INT,			// ��������
        BOOL,			// ��������
        CHAR,			// �ַ�
        STRING,			// �ַ���
        SYMBOL,         // �Ϸ��ķ���
        NONE,		    // ������
        ERROR,		    // ����
        ENDOFFILE	    // �ļ�����
    };

	enum TokenType_Detail
	{
		Token_And = 256, Token_Break, Token_Do, Token_Else, Token_ElseIf, Token_End,
		Token_False, Token_For, Token_Function, Token_If, Token_In,
		Token_Local, Token_Nil, Token_Not, Token_Or, Token_Repeat,
		Token_Return, Token_Then, Token_True, Token_Until, Token_While,
		Token_Id, Token_String, Token_Number,
		Token_Equal, Token_NotEqual, Token_LessEqual, Token_GreaterEqual,
		Token_Concat, Token_VarArg, Token_EOF,
		Token_LeftRoundBracket,Token_RightRoundBracket,
		Token_LeftSquareBracket, Token_RightSquareBracket,
		Token_LeftBrace, Token_RightBrace,
		Token_Comma, Token_Semicolon,
		Token_Assign
	};
    struct Token
    {
        TokenType kind;				// Token������
        string lexeme;				// Token��ֵ
        unsigned row;	   	        // ��ǰ��

		bool compare(TokenType_Detail td)  {
			return (this->lexeme == detailType2Lexeme(td));
		}
		bool compare(string lexeme)  {
			return (this->lexeme == lexeme);
		}
		explicit Token() = default;
		Token(const Token& t)  {
			kind = t.kind;
			lexeme = t.lexeme;
			row = t.row;
		}
		Token(TokenType_Detail type_det)  {
			kind = SYMBOL;
			lexeme = detailType2Lexeme(type_det);
		}
		Token(TokenType k, string l = "")  {
			kind = k;
			lexeme = l;
		}
    };
	
    void initKeyWords();
    void initSymbols();

private:
	string _stringCode;
	bool _parseString;                      //�ǽ����ļ�����string
    string _strLineBuffer;					// ������, ����Դ�����е�һ������
    unsigned _nBufferPos;					// �����е�ָ��
    unsigned _row;					     	// ���浱ǰ��������Դ�����е��к�
    ifstream _fIn;					    	// Դ�����ļ�������������
    char nextChar();					    // ���ػ������е���һ���ַ�
    void rollBack();					    // �ع�������
    TokenType searchReserved(string &s);	// ���ҹؼ���

	static string detailType2Lexeme(TokenType_Detail detail);

	State procStartState(char ch, Token& token);
	bool procIntState(char ch, Token& token);
	bool procIdentityState(char ch, Token& token);
	void procStringState(char ch, Token& token, Scanner::State& state);
	void procCharState(char ch, Token& token, Scanner::State& state);
	void procSymbolState(char ch, Token& token, Scanner::State& state);
	void procCommentState(char ch, Token& token, Scanner::State& state);

public:
    Scanner();
    int openFile(string filename);
    void closeFile();
    Token nextToken();					  // ������һ��Token
    void resetRow();
	void setParseString(bool b) { _parseString = b; }
	void setStringCode(string& str)  { _stringCode = str; }
};

#endif
