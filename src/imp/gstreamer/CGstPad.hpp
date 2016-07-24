/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    CGstPad.hpp
 * @date    16.07.16
 * @author  Hlieb Romanov
 * @brief   GStreamer element pad wrapper
 ************************************************************************/
#pragma once

#include <gst/gst.h>
#include <string>
#include <boost/function.hpp>

typedef boost::function< GstPadProbeReturn ( GstPadProbeInfo* info ) > GstPadProbeFunc;

/**
 * A kind of conector between two elements.
 */
class CGstPad
{
public:
   /**
    * Constructs invalid pad
    */
   CGstPad( void );

   ~CGstPad( void );

   /**
    * Create CGstPad object from raw pointer.
    * @param pad - raw gstreamer pad
    * @param takeOwnership - if set to true wrapper will not make gst_object_ref() on raw pointer
    */
   explicit CGstPad( GstPad* pad, bool takeOwnership = false );

   CGstPad( const CGstPad& other );
   CGstPad& operator=( CGstPad other );
   friend void swap( CGstPad& first, CGstPad& second );

   bool isValid( void ) const;

   /**
    * Get the name of this pad.
    */
   std::string getName( void ) const;

   /**
    * Get the pad which is connect to this one.
    */
   CGstPad getPeerPad( void ) const;

   /**
    * Send event to this pad.
    * @return true if event was handled.
    */
   bool sendEvent( GstEvent* event );

   /**
    * Lint this pad to another one.
    * Dataflow direction as follows: this_pad -> peer_pad
    * @return true if link was performed.
    */
   bool link( CGstPad& other );

   /**
    * Unlink this pad and another one
    */
   bool unlink( CGstPad& other );

   /**
    * Get raw pointer of this pad.
    * Do not call gst_object_unref() on it. 
    * It still belongs to this wrapper.
    */
   GstPad* raw( void );

   /**
    * Set callback function (probe) for gstreamer buffer or event on this pad.
    * Callback function will be called in streaming thread, so be carefull.
    * @param mask - what kind of probe to set
    * @param callback - callback function
    * @return pad probe id
    */
   unsigned long addProbe( GstPadProbeType mask, const GstPadProbeFunc& callback );

   /**
    * Remove pad probe by ID.
    */
   void removeProbe( unsigned long probeId );

private:
   void extractName( void );
   static GstPadProbeReturn probeCallback( GstPad* pad, GstPadProbeInfo* info, gpointer user_data );

private:
   GstPad* mPad;
   std::string mName;
   GstPadProbeFunc mPadProbeFunc;
};