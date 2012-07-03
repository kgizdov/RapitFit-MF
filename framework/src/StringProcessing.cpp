/**
  @class StringProcessing

  Collection of static functions for string processing

  @author Benjamin M Wynne bwynne@cern.ch
  @date 2009-10-02
  */

//	ROOT Headers
#include "TString.h"
//	RapidFit Headers
#include "StringProcessing.h"
//	System Headers
#include <iostream>
#include <sstream>
#include <algorithm>
#include <time.h>

using namespace::std;

string StringProcessing::TimeString()
{
	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	int min = timeinfo->tm_min;
	int hr = timeinfo->tm_hour;
	int day = timeinfo->tm_mday;
	int month = 1+timeinfo->tm_mon;
	int year = 1900+timeinfo->tm_year;

	stringstream time_stream;
	time_stream << year;
	if( month < 10 ) time_stream << "0";
	time_stream << month;
	if( day < 10 ) time_stream << "0";
	time_stream << day << "_";
	if( hr < 10 ) time_stream << "0";	
	time_stream << hr;
	if( min < 10 ) time_stream << "0";
	time_stream << min ;

	string returnable_time = time_stream.str();

	return returnable_time;
}

//Split a string every time you find a given character
vector<string> StringProcessing::SplitString( const string Input, const char SplitCharacter )
{
	vector<string> splitParts;

	string Loop_Str = Input;

	while(true)
	{
		//Search for the character
		int position = CharacterPosition( Loop_Str, SplitCharacter );

		if ( position == -1 )
		{
			//Ignore empty strings
			if ( Input != "" )
			{
				splitParts.push_back( Loop_Str );
			}

			//If it's not found, you've reached the end
			break;
		}
		else
		{
			//Split the string at the character postion
			string tempString( Loop_Str, 0, unsigned(position) );
			string newInput( Loop_Str, unsigned(position + 1) );

			//Ignore empty strings
			if ( tempString != "" )
			{
				splitParts.push_back( tempString );
			}
			Loop_Str = newInput;
		}
	}

	return splitParts;
}

//Return the position of the first instance of a character in a string
int StringProcessing::CharacterPosition( const string Input, const char SearchCharacter )
{
	for (unsigned int characterIndex = 0; characterIndex < Input.size(); ++characterIndex)
	{
		//Look for the character
		if ( Input[characterIndex] == SearchCharacter )
		{
			return int(characterIndex);
		}

		//If you get to the end, character not found
		if ( characterIndex == Input.size() - 1 )
		{
			return -1;
		}
	}
	return -1;
}

//Return the position of all instances of a string in another string
vector<int> StringProcessing::StringPositions( const string Input, const string SearchString )
{
	vector<int> positions;
	int numberDiscarded = 0;

	string Loop_Str = Input;

	while (true)
	{
		int firstCharacterPosition = CharacterPosition( Loop_Str, SearchString[0] );
		if ( firstCharacterPosition == -1 )
		{
			//If you can't find the first character of the string, you won't find the string!
			return positions;
		}
		else
		{
			bool found = true;
			for (unsigned int characterIndex = 0; characterIndex < SearchString.size(); ++characterIndex )
			{
				//Compare each subsequent character
				if ( Input[ characterIndex + unsigned(firstCharacterPosition) ] != SearchString[characterIndex] )
				{
					found = false;
					break;
				}
			}

			if (found)
			{
				positions.push_back( firstCharacterPosition + numberDiscarded );
			}

			//Search the rest of the string
			string tempString( Loop_Str, unsigned(firstCharacterPosition + 1) );
			numberDiscarded += unsigned(firstCharacterPosition + 1);
			Loop_Str = tempString;
		}
	}

	return positions;
}

//Remove any instances of a particular character in a string
void StringProcessing::RemoveCharacter( string & Input, const char SearchCharacter )
{
	char* passedCharacters = new char[ Input.size() + 1 ];
	int addedCharacters = 0;

	for (unsigned int characterIndex = 0; characterIndex < Input.size(); ++characterIndex )
	{
		if ( Input[characterIndex] != SearchCharacter )
		{
			passedCharacters[addedCharacters] = Input[characterIndex];
			++addedCharacters;
		}
	}

	passedCharacters[addedCharacters] = '\0';
	Input = passedCharacters;
}

//Replace any instances of a particular character in a string
string StringProcessing::ReplaceString( const string & Input, const string FindString, const string ReplaceString )
{
	string output;

	//Search the input character by character
	for (unsigned int characterIndex = 0; characterIndex < Input.size(); ++characterIndex )
	{
		//Find out if the full search string is present
		bool isInstance = true;
		for (unsigned int testIndex = 0; testIndex < FindString.size(); ++testIndex )
		{
			if ( Input[ characterIndex + testIndex ] != FindString[testIndex] )
			{
				isInstance = false;
				break;
			}
		}

		//Replace or push back
		if (isInstance)
		{
			for (unsigned int replaceIndex = 0; replaceIndex < ReplaceString.size(); ++replaceIndex )
			{
				output.push_back( ReplaceString[replaceIndex] );
			}

			characterIndex += unsigned(FindString.size() - 1);
		}
		else
		{
			output.push_back( Input[characterIndex] );
		}
	}

	return output;
}

//Remove white space from passed lines
void StringProcessing::RemoveWhiteSpace( vector<string> & newContent )
{
	vector<string> output;

	for (unsigned int lineIndex = 0; lineIndex < newContent.size(); ++lineIndex )
	{
		//Remove tabs
		RemoveCharacter( newContent[lineIndex], '\t' );

		//Remove empty lines
		if ( newContent[lineIndex] != "" )
		{
			output.push_back( newContent[lineIndex] );
		}
	}

	newContent = output;
}

//Return a vector containing all the unique strings from the two input vectors
vector<string> StringProcessing::CombineUniques( const vector<string> VectorOne, const vector<string> VectorTwo )
{
	vector<string> result;
	vector<string>::const_iterator stringIterator;

	//Don't assume VectorOne is unique
	for ( stringIterator = VectorOne.begin(); stringIterator != VectorOne.end(); ++stringIterator )
	{
		if ( VectorContains( &result, &(*stringIterator) ) == -1 )
		{
			result.push_back( *stringIterator );
		}
	}

	//Now add in VectorTwo
	for ( stringIterator = VectorTwo.begin(); stringIterator != VectorTwo.end(); ++stringIterator )
	{
		if ( VectorContains( &result, &(*stringIterator) ) == -1 )
		{
			result.push_back( *stringIterator );
		}
	}

	return result;
}

vector<string> StringProcessing::RemoveCommon( const vector<string> VectorOne, const vector<string> VectorTwo )
{
	vector<string> result;
	vector<string> erasable;
	vector<string>::const_iterator stringIterator;

	vector<vector<string>::const_iterator> remove_list;

	//Don't assume VectorOne is unique
	for ( stringIterator = VectorOne.begin(); stringIterator != VectorOne.end(); ++stringIterator )
	{
		if ( VectorContains( &result, &(*stringIterator) ) == -1 )
		{
			result.push_back( *stringIterator );
		}
		else
		{
			erasable.push_back( *stringIterator );
		}
	}

	//Now add in VectorTwo
	for ( stringIterator = VectorTwo.begin(); stringIterator != VectorTwo.end(); ++stringIterator )
	{
		if ( VectorContains( &result, &(*stringIterator) ) == -1 )
		{
			result.push_back( *stringIterator );
		}
	}

	for( vector<string>::iterator e_i = erasable.begin(); e_i != erasable.end(); ++e_i )
	{
		StringProcessing::RemoveElement( result, *e_i );
	}

	return result;
}

void StringProcessing::RemoveElement( vector<string>& Input, string Element )
{
	vector<string>::iterator obj_i = Input.begin();
	vector<vector<string>::iterator> element_i;
	vector<string>::iterator end_i = Input.end();

	for( ; obj_i != end_i; ++obj_i )
	{
		if( *obj_i == Element ) element_i.push_back( obj_i );
	}

	for( vector<vector<string>::iterator>::iterator remov_i = element_i.begin(); remov_i != element_i.end(); ++remov_i )
	{
		Input.erase( *remov_i );
	}
}

//Return the position of a search string within a vector of strings, or -1 if not found
int StringProcessing::VectorContains( vector<string> const* InputVector, string const* SearchString )
{
	if( InputVector->empty() ) return -1;
	vector<string>::const_iterator begin = InputVector->begin();
	vector<string>::const_iterator ending = InputVector->end();
	vector<string>::const_iterator result = find( begin, ending, *SearchString);
	int position=int( result - InputVector->begin() );
	if( position < (int) InputVector->size() ) return position;

	//If you've got this far, it wasn't found
	return -1;
}

//Return the position of a search string within a vector of strings, or -1 if not found
int StringProcessing::VectorContains( const vector<string>& InputVector, const string& SearchString )
{
	if( InputVector.empty() ) return -1;
	vector<string>::const_iterator begin = InputVector.begin();
	vector<string>::const_iterator ending = InputVector.end();
	vector<string>::const_iterator result = find( begin, ending, SearchString);
	int position=int( result - InputVector.begin() );
	if( position < (int) InputVector.size() ) return position;

	//If you've got this far, it wasn't found
	return -1;
}

//Return the a TString object which is composed of the selected strings within the vector
TString StringProcessing::CondenseStrings( const vector<string>& input_vec, const int lolim=-1, const int hilim=-1 )
{
	int temp_int = int( input_vec.size() );
	int temp_hilim = hilim;
	int temp_lolim = lolim;

	if( ( temp_hilim > temp_int ) || (temp_hilim < 0) ) temp_hilim = temp_int;
	if( temp_lolim < 0 ) temp_lolim = 0;

	TString Returnable_String( "" );
	for( int iter = temp_lolim; iter < temp_hilim ; ++iter )
	{
		Returnable_String.Append( input_vec[(unsigned)iter] );
	}
	return Returnable_String;
}

vector<TString> StringProcessing::GetStringContaining( const vector<TString> list, const TString search_str )
{
	vector<TString> output_list;

	for( unsigned int i=0; i< list.size(); ++i )
	{
		string input = list[i].Data();
		size_t found = input.find( search_str.Data() );
		if( found != string::npos )
		{
			output_list.push_back( list[i] );
		}
	}

	return output_list;
}

vector<TString> StringProcessing::StripStrings( const vector<TString> list, const TString ext )
{
	vector<TString> output_list;
	string remove = ext.Data();
	for( unsigned int i=0; i< list.size(); ++i )
	{
		string object = list[i].Data();
		size_t found = object.find( remove );
		TString TObject_Str;
		if( found != string::npos )
		{
			TObject_Str = object.substr( 0, object.length()-remove.length() ).c_str();
		} else {
			TObject_Str = object.c_str();
		}
		output_list.push_back( TObject_Str );
	}
	return output_list;
}

vector<string> StringProcessing::Convert( const vector<TString> input )
{
	vector<string> output;
	for( unsigned int i=0; i< input.size(); ++i )
	{
		output.push_back( input[i].Data() );
	}
	return output;
}
//      Chosen to use sprintf as itoa is not supported everywhere...

string StringProcessing::AddNumberToLeft( const string& input_str, const int& num2add )
{
	stringstream temp_stream;
	temp_stream << num2add ;
	string numberstr;
	numberstr.append( temp_stream.str() );
	numberstr.append(input_str);
	return numberstr;
}

string StringProcessing::RemoveFirstNumber( const string& input_str )
{
	stringstream output_stream;
	for( unsigned int i=1; i<input_str.size(); ++i )
	{
		output_stream << input_str.at(i);
	}
	return output_stream.str();
}

vector<string> StringProcessing::FillList( const int max, const int min )
{
	vector<string> returnable_list;
	for( int i=min; i<= max; ++i )
	{
		char numberchar[1];
		sprintf( numberchar, "%d", i );
		string temp( numberchar );
		returnable_list.push_back( temp );
	}
	return returnable_list;
}

int StringProcessing::GetNumberOnLeft( const string& input_str )
{
	string input_num; input_num+=input_str[0];
	return atoi( &(input_num[0]) );	//Just take first character not whole string
}

string StringProcessing::AddNames( const string& input1, const string& input2)
{
	if( !input1.empty() && !input2.empty() )
	{
		return input1+"+"+input2;
	}
	if( input1.empty() && input2.empty() )
	{
		return "unknown+unknown";
	}
	if( input1.empty() )
	{
		return "unknown+"+input2;
	}
	if( input2.empty() )
	{
		return input1+"+unknown";
	}
	return "";
}

string StringProcessing::MultNames( const string& input1, const string& input2 )
{
	if( !input1.empty() && !input2.empty() )
	{
		return input1+":"+input2;
	}
	if( input1.empty() && input2.empty() )
	{
		return "unknown:unknown";
	}
	if( input1.empty() )
	{
		return "unknown:"+input2;
	}
	if( input2.empty() )
	{
		return input1+":unknown";
	}
	return "";
}

TString StringProcessing::Clean( const TString input )
{
	string temp(input.Data());

	replace(temp.begin(), temp.end(), '.', '_');
	replace(temp.begin(), temp.end(), '/', '_');
	replace(temp.begin(), temp.end(), '\\', '_' );
	replace(temp.begin(), temp.end(), ' ', '_' );
	replace(temp.begin(), temp.end(), '(', '_' );
	replace(temp.begin(), temp.end(), ')', '_' );

	vector<string::iterator> remove_list;
	string::iterator ch=temp.begin();
	for( unsigned int i=0; i< temp.size(); ++i, ++ch )
	{
		if( i+1 != temp.size() )
		{
			if( temp[i] == temp[i+1] )
			{
				if( temp[i] == '_' )    remove_list.push_back( ch );
			}
		}
	}
	if( temp[0] == '_' ) remove_list.push_back( temp.begin() );

	for( unsigned int i=0; i< remove_list.size(); ++i )
	{
		temp.erase( remove_list[i] );
	}

	return TString( temp.c_str() );
}

//      Is a TString empty ?
bool StringProcessing::is_empty( const TString input )
{
	string temp("");
	if ( temp.compare( string(input.Data()) ) == 0 ) return true;
	return false;
}

string StringProcessing::LatexSafe( const string input )
{               
	string temp(input);
	size_t found=0;

	bool modified=false;
	while( found != string::npos )
	{
		size_t start_found=found;
		if( start_found == 0 ) found=temp.find("_");
		else found=temp.find("_",start_found+2);

		if( found != string::npos )
		{
			modified=true;
			temp.insert(found,"\\");
		}
	}

	if( !modified ) return input;
	else return temp;
}

string StringProcessing::LatexSafe( const TString input )
{                   
	string temp(input.Data());
	return LatexSafe(temp);
}

