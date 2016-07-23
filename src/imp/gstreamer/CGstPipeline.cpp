/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    CGstPipeline.cpp
 * @date    19.07.16
 * @author  Hlieb Romanov
 * @brief   GStreamer pipeline wrapper
 ************************************************************************/
#include "CGstPipeline.hpp"

static GstElement* pipelineParseWrapper( const std::string& pipelineStruct )
{
   GError* error = NULL;
   GstElement* pipeline = gst_parse_launch( pipelineStruct.c_str(), &error );
   if ( error )
   {
      /**
       * TODO: handle error somehow
       */
      g_error_free( error );
   }
   if ( pipeline )
   {
      pipeline = ( GST_IS_PIPELINE( pipeline ) ) ? pipeline : NULL;
   }
   return pipeline;
}

CGstPipeline::CGstPipeline( void )
   : CGstElement( gst_pipeline_new( NULL ), true )
{
   addBusWatch();
}

CGstPipeline::CGstPipeline( const std::string& pipelineStruct )
   : CGstElement( pipelineParseWrapper( pipelineStruct ), true )
{
   addBusWatch();
}

CGstPipeline::~CGstPipeline( void )
{

}

bool CGstPipeline::addElement( CGstElement& element )
{
   assert( isValid() );
   GstBin* pipeline = GST_BIN( raw() );
   return ( gst_bin_add( pipeline, element.raw() ) != FALSE );
}

CGstPipeline& CGstPipeline::operator<<( CGstElement& element )
{
   assert( isValid() );
   GstBin* pipeline = GST_BIN( raw() );
   gst_bin_add( pipeline, element.raw() );
   return *this;
}

CGstElement CGstPipeline::getElementByName( const std::string& elementName )
{
   assert( isValid() );
   GstBin* pipeline = GST_BIN( raw() );
   return CGstElement( gst_bin_get_by_name( pipeline, elementName.c_str() ) );
}

void CGstPipeline::setBusCallback( const GstBusCallback& callback )
{
   boost::lock_guard<boost::mutex> guard( mCallbackGuard );
   mCallback = callback;
}

void CGstPipeline::addBusWatch( void )
{
   GstPipeline* pipeline = GST_PIPELINE( raw() );
   mBus = GstBusPtr( gst_pipeline_get_bus( pipeline ), &CGstPipeline::deallocateBus );
   gst_bus_add_watch( mBus.get(), busCallback, this );
}

void CGstPipeline::deallocateBus( GstBus* bus )
{
   gst_bus_remove_watch( bus );
   gst_object_unref( bus );
}

gboolean CGstPipeline::busCallback( GstBus* bus, GstMessage* message, gpointer user_data )
{
   CGstPipeline* pipeline = reinterpret_cast<CGstPipeline*>( user_data );
   boost::lock_guard<boost::mutex> guard( pipeline->mCallbackGuard );
   if ( pipeline->mCallback )
   {
       pipeline->mCallback( bus, message );
   }
   // always return TRUE. Otherwise, GLIB will delete this handler
   return TRUE;
}