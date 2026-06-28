#pragma once

#include <iostream>
#include <string>
#include <vector>

class CSVReader
{
public:
	CSVReader();
	~CSVReader();

	void SetDelimiter( char cDelimiter );
	char GetDelimiter() const;

	void Close();
	bool IsOpened() const;

	void RemoveQuotes( std::string& strParam );
	void Trim( std::string& strParam );

	void ReadCSV( const std::string& strFileName );
	
	void ParseHeader( const std::string& strHeader );
	void ParseLine( const std::string& strLine );

	int GetColumnCount() const;
	std::string GetColumn( int iCol ) const;
	int FindColumn( const std::string& strColumn ) const;
	int GetRowCount() const;
	std::string GetField( const std::string& strColumn, int iRow ) const;

private:
	char                                   m_cDelimiter;
	std::vector<std::string>               m_vHeader;
	std::vector<std::vector<std::string>>  m_vValues;
	bool                                   m_bOpened;
};

