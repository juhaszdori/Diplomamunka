#include "CSVReader.h"
#include <sstream>
#include <fstream>

CSVReader::CSVReader()
	: m_cDelimiter( ',' )
	, m_bOpened( false )
{
}

CSVReader::~CSVReader()
{
	Close();
}

void CSVReader::SetDelimiter( char cDelimiter )
{
	m_cDelimiter = cDelimiter;
}

char CSVReader::GetDelimiter() const
{
	return m_cDelimiter;
}

bool CSVReader::IsOpened() const
{
	return m_bOpened;
}

void CSVReader::Close()
{
	m_vHeader.clear();
	m_vValues.clear();

	m_bOpened = false;
}

void CSVReader::RemoveQuotes( std::string& strParam )
{
	strParam.erase( remove( strParam.begin(), strParam.end(), '\"' ), strParam.end() );
}

void CSVReader::Trim( std::string& strParam )
{
	//strParam.erase( 0, strParam.find_first_not_of( " \n\r\t" ) );
	//strParam.erase( strParam.find_last_not_of( " \n\r\t" ) + 1 );

	std::size_t first = strParam.find_first_not_of( " \n\r\t" );

	if( first == std::string::npos )
	{
		strParam.clear();
		return;
	}

	std::size_t last = strParam.find_last_not_of( " \n\r\t" );

	strParam.erase( last + 1 );
	strParam.erase( 0, first );
}

void CSVReader::ReadCSV( const std::string& strFileName )
{
	std::ifstream file( strFileName );

	if( !file.is_open() )
	{
		throw std::runtime_error( "Failed to open file: " + strFileName );
	}

	if( IsOpened() )
		Close();

	std::string strHeader;
	std::getline( file, strHeader );

	ParseHeader( strHeader );

	std::string strLine;
	while( std::getline( file, strLine ) )
	{
		ParseLine( strLine );
	}

	m_bOpened = true;
}

void CSVReader::ParseHeader( const std::string& strHeader )
{
	std::string strItem;
	std::stringstream ss( strHeader );

	while( std::getline( ss, strItem, m_cDelimiter) )
	{
		Trim( strItem );
		RemoveQuotes( strItem );

		m_vHeader.push_back( strItem );
	}
}

void CSVReader::ParseLine( const std::string& strLine )
{
	std::vector<std::string> vValues;

	std::stringstream ss( strLine );
	std::string strItem;

	while( std::getline( ss, strItem, m_cDelimiter ) )
	{
		Trim( strItem );
		RemoveQuotes( strItem );

		vValues.push_back( strItem );
	}

	m_vValues.push_back( vValues );
}

int CSVReader::GetColumnCount() const
{
	return m_vHeader.size();
}

std::string CSVReader::GetColumn( int iCol ) const
{
	return m_vHeader.at( iCol );
}

int CSVReader::FindColumn( const std::string& strColumn ) const
{
	for( int i = 0; i < m_vHeader.size(); i++ )
	{
		if( m_vHeader.at( i ) == strColumn )
			return i;
	}

	return -1;
}

int CSVReader::GetRowCount() const
{
	return m_vValues.size();
}

std::string CSVReader::GetField( const std::string& strColumn, int iRow ) const
{
	int iCol = FindColumn( strColumn );

	if( iCol == -1 )
	{
		throw std::runtime_error( "Could not find column: " + strColumn );
	}

	return m_vValues.at( iRow ).at( iCol );
}