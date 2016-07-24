/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    CGstPipeline.hpp
 * @date    19.07.16
 * @author  Hlieb Romanov
 * @brief   GStreamer pipeline wrapper
 ************************************************************************/
#pragma once
#include <boost/thread.hpp>
#include "CGstElement.hpp"

typedef boost::function<void ( GstBus*, GstMessage* )> GstBusCallback; ///< The bus callback function prototype
typedef boost::shared_ptr<GstBus> GstBusPtr;

class CGstPipeline : public CGstElement
{
public:
   CGstPipeline( void );

   /**
    * Construct a pipeline from the pipeline description string.
    * The syntax of the pipeline description the smae as in the gst-launch CLI tool.
    * @param pipelineStruct - the pipeline description string, for example "fakesrc ! audioconvert ! fakesink"
    */
   explicit CGstPipeline( const std::string& pipelineStruct );
   virtual ~CGstPipeline( void );

   /**
    * Add specified element to the pipeline.
    */
   bool addElement( CGstElement& element );

   /**
    * @sa addElement()
    */
   CGstPipeline& operator<<( CGstElement& element );

   /**
    * Get the element from this pipeline by it's name.
    */
   CGstElement getElementByName( const std::string& elementName );

   /**
    * Set the bus callback function to receive notifications
    * from the pipeline.
    */
   void setBusCallback( const GstBusCallback& callback );

private:
   /**
    * Helper function to regiser the bus callback.
    */
   void addBusWatch( void );

   /**
    * Helper function to remove the bus callback.
    */
   void removeBusWatch( void );

private:
   /**
    * Static bus callback that redirects calls to the user specified callback function.
    */
   static gboolean busCallback( GstBus* bus, GstMessage* message, gpointer user_data );

   /**
    * Deallocator for the GstBus raw pointer.
    */
   static void deallocateBus( GstBus* bus );
private:
   GstBusPtr mBus;
   GstBusCallback mCallback;
   boost::mutex mCallbackGuard;
};