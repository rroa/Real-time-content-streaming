#pragma once

#ifndef _MANIFEST_DOWNLOAD_H_
#define _MANIFEST_DOWNLOAD_H_

#include <string>
#include <fstream>
#include "BaseJob.h"
#include "ReportErrors.h"
#include "XMLExtensions.h"

namespace Solidus
{
	class ManifestDownload : public BaseJob
	{
	public:
		ManifestDownload( const std::string& name, const std::string& fileName, const JobPriority::Base& priority )
			: BaseJob( name, priority )	
			, m_fileName( fileName )
		{}
		void Execute()
		{
			trace( "Executing: " << m_name );
			get_file( m_fileName );
		}

		void CallBack()
		{
			trace( "Loaded file for job " << m_name );
			
			pugi::xml_document mapsDocument;
			pugi::xml_parse_result mapsDocumentResult = mapsDocument.load_file( m_fileName.c_str() );

			if( !mapsDocumentResult ) // Error Checking.
			{
				std::stringstream errorMessage;
				errorMessage << m_fileName
					<< " was not loaded properly. Check your map definition files.";

				ReportError( errorMessage.str() );
				return;				
			}

			XMLNode rootNode = mapsDocument.child( "manifest" );
			if( !rootNode ) // Error Checking.
			{
				std::stringstream errorMessage;
				errorMessage << "The file "
					<< m_fileName
					<< " was skipped because the root node \"maps\" was not found.";

				ReportError( errorMessage.str() );
				//continue;
			}

			pugi::xpath_query queryTileTypes("/manifest/packages/package");			
			pugi::xpath_node_set childs = queryTileTypes.evaluate_node_set( mapsDocument );

			for( auto elementFromQuery = childs.begin(); elementFromQuery != childs.end(); ++elementFromQuery )
			{
				XMLNode childNode = elementFromQuery->node();
				std::string priority = GetStringXMLAttribute( childNode, "priority" );
				if( priority == "HIGH" )
				{
					for( auto file = childNode.begin(); file != childNode.end(); ++ file )
					{						
						std::string fileName = GetStringXMLAttribute( *file, "file" );
						trace("Created job to download file " << fileName);
						Solidus::JobManager::Instance().CreateDownloadJob( fileName, JobPriority::High );
					}
				}
				else if( priority == "MEDIUM" )
				{
					for( auto file = childNode.begin(); file != childNode.end(); ++ file )
					{
						std::string fileName = GetStringXMLAttribute( *file, "file" );
						trace("Created job to download file " << fileName);
						Solidus::JobManager::Instance().CreateDownloadJob( fileName, JobPriority::Average );
					}
				}
				else if( priority == "LOW" )
				{
					for( auto file = childNode.begin(); file != childNode.end(); ++ file )
					{
						std::string fileName = GetStringXMLAttribute( *file, "file" );
						trace("Created job to download file " << fileName);
						Solidus::JobManager::Instance().CreateDownloadJob( fileName, JobPriority::Low );
					}
				}				
			}
		}

	private:
		std::string m_fileName;	
	};
}

#endif