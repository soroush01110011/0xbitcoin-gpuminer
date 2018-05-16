/*********************************************************************
 * NAN - Native Abstractions for Node.js
 *
 * Copyright (c) 2017 NAN contributors
 *
 * MIT License <https://github.com/nodejs/nan/blob/master/LICENSE.md>
 ********************************************************************/

#include "addon.h"

HybridMiner* miner::hybridminer = nullptr;

//call C++ dtors:
void miner::cleanup( void* p )
{
  delete reinterpret_cast<HybridMiner*>( p );
}

miner::Miner::Miner( Nan::Callback *callback ) noexcept
  : AsyncWorker( callback, "miner::Miner" )
{
}

// This function runs in a thread spawned by NAN
void miner::Miner::Execute()
{
  if( hybridminer )
  {
    hybridminer->run(); // blocking call
  }
  else
  {
    SetErrorMessage( "{error: 'no hybridminer!'}" );
  }
}

// Executed when the async work is complete
// this function will be run inside the main event loop
// so it is safe to use V8 again
void miner::Miner::HandleOKCallback()
{
  // HandleScope scope;

  // v8::Local<v8::Value> argv[] = {
  //   Null()
  // };

  // callback->Call( 2, argv, async_resource );
}

// Run an asynchronous function
//  First and only parameter is a callback function
//  receiving the solution when found
NAN_METHOD( miner::run )
{
  Nan::Callback *callback = new Nan::Callback( Nan::To<v8::Function>( info[0] ).ToLocalChecked() );
  Nan::AsyncQueueWorker( new Miner( callback ) );
}

NAN_METHOD( miner::stop )
{
  hybridminer->stop();
  info.GetReturnValue().SetUndefined();
}

NAN_METHOD( miner::getTarget )
{
  info.GetReturnValue().Set( Nan::New<v8::String>( std::string( BigUnsignedInABase( MinerState::getTarget(), 10 ) ) ).ToLocalChecked() );
}

NAN_METHOD( miner::getMaximumTarget )
{
  info.GetReturnValue().Set( Nan::New<v8::String>( MinerState::getMaximumTarget() ).ToLocalChecked() );
}

NAN_METHOD( miner::getChallenge )
{
  info.GetReturnValue().Set( Nan::New<v8::String>( MinerState::getChallenge() ).ToLocalChecked() );
}

NAN_METHOD( miner::getPreviousChallenge )
{
  info.GetReturnValue().Set( Nan::New<v8::String>( MinerState::getPreviousChallenge() ).ToLocalChecked() );
}

NAN_METHOD( miner::getPoolAddress )
{
  info.GetReturnValue().Set( Nan::New<v8::String>( MinerState::getPoolAddress() ).ToLocalChecked() );
}

NAN_METHOD( miner::setPrefix )
{
  Nan::MaybeLocal<v8::String> inp = Nan::To<v8::String>( info[0] );
  if( !inp.IsEmpty() )
  {
    MinerState::setPrefix( std::string( *Nan::Utf8String( inp.ToLocalChecked() ) ) );
  }
  info.GetReturnValue().SetUndefined();
}

NAN_METHOD( miner::getPrefix )
{
  info.GetReturnValue().Set( Nan::New<v8::String>( MinerState::getPrefix() ).ToLocalChecked() );
}

NAN_METHOD( miner::setAddress )
{
  Nan::MaybeLocal<v8::String> inp = Nan::To<v8::String>( info[0] );
  if( !inp.IsEmpty() )
  {
    MinerState::setAddress( std::string( *Nan::Utf8String( inp.ToLocalChecked() ) ) );
  }
  info.GetReturnValue().SetUndefined();
}

NAN_METHOD( miner::getAddress )
{
  info.GetReturnValue().Set( Nan::New<v8::String>( MinerState::getAddress() ).ToLocalChecked() );
}

NAN_METHOD( miner::getCustomDiff )
{
  v8::Local<v8::Boolean> ret = Nan::New( MinerState::getCustomDiff() );
  info.GetReturnValue().Set( ret );
}

NAN_METHOD( miner::setDiff )
{
  Nan::MaybeLocal<v8::Uint32> inp = Nan::To<v8::Uint32>( info[0] );
  if( !inp.IsEmpty() )
  {
    MinerState::setDiff( inp.ToLocalChecked()->Uint32Value() );
  }
  info.GetReturnValue().SetUndefined();
}

NAN_METHOD( miner::getDiff )
{
  info.GetReturnValue().Set( Nan::New<v8::Number>( static_cast<double>(MinerState::getDiff()) ) );
}

NAN_METHOD( miner::getPoolUrl )
{
  info.GetReturnValue().Set( Nan::New<v8::String>( MinerState::getPoolUrl() ).ToLocalChecked() );
}

NAN_METHOD( miner::resetHashCounter )
{
  MinerState::resetCounter();
  info.GetReturnValue().SetUndefined();
}

NAN_METHOD( miner::incSolCount )
{
  Nan::MaybeLocal<v8::Uint32> inp = Nan::To<v8::Uint32>( info[0] );
  if( !inp.IsEmpty() )
  {
    MinerState::incSolCount( inp.ToLocalChecked()->Uint32Value() );
  }
  info.GetReturnValue().SetUndefined();
}

NAN_METHOD( miner::getSolution )
{
  info.GetReturnValue().Set( Nan::New<v8::String>( MinerState::getSolution() ).ToLocalChecked() );
}

NAN_METHOD( miner::log )
{
  Nan::MaybeLocal<v8::String> inp = Nan::To<v8::String>( info[0] );
  if( !inp.IsEmpty() )
  {
    MinerState::pushLog( std::string( *Nan::Utf8String( inp.ToLocalChecked() ) ) );
  }
  info.GetReturnValue().SetUndefined();
}

NAN_METHOD( miner::getTokenName )
{
  info.GetReturnValue().Set( Nan::New<v8::String>( MinerState::getTokenName() ).ToLocalChecked() );
}

NAN_METHOD( miner::getSubmitStale )
{
  v8::Local<v8::Boolean> ret = Nan::New( MinerState::getSubmitStale() );
  info.GetReturnValue().Set( ret );
}

NAN_METHOD( miner::keccak256 )
{
  Nan::MaybeLocal<v8::String> inp = Nan::To<v8::String>( info[0] );
  if( !inp.IsEmpty() )
  {
    info.GetReturnValue().Set( Nan::New<v8::String>( MinerState::keccak256( std::string( *Nan::Utf8String( inp.ToLocalChecked() ) ) ) ).ToLocalChecked() );
  }
  else
  {
    info.GetReturnValue().SetUndefined();
  }
}

// Defines the functions our add-on will export
NAN_MODULE_INIT( miner::Init )
{
  Set( target
       , Nan::New<v8::String>( "run" ).ToLocalChecked()
       , Nan::New<v8::FunctionTemplate>( run )->GetFunction() );

  Set( target
       , Nan::New<v8::String>( "stop" ).ToLocalChecked()
       , Nan::New<v8::FunctionTemplate>( stop )->GetFunction() );

  Set( target
       , New<v8::String>( "getTarget" ).ToLocalChecked()
       , New<v8::FunctionTemplate>( getTarget )->GetFunction()
       );

  Set( target
       , New<v8::String>( "getMaximumTarget" ).ToLocalChecked()
       , New<v8::FunctionTemplate>( getMaximumTarget )->GetFunction()
       );

  Set( target
       , New<v8::String>( "getChallenge" ).ToLocalChecked()
       , New<v8::FunctionTemplate>( getChallenge )->GetFunction()
       );

  Set( target
       , New<v8::String>( "getPreviousChallenge" ).ToLocalChecked()
       , New<v8::FunctionTemplate>( getPreviousChallenge )->GetFunction()
       );

  Set( target
       , New<v8::String>( "getPoolAddress" ).ToLocalChecked()
       , New<v8::FunctionTemplate>( getPoolAddress )->GetFunction()
       );

  Set( target
       , New<v8::String>( "setPrefix" ).ToLocalChecked()
       , New<v8::FunctionTemplate>( setPrefix )->GetFunction()
       );

  Set( target
       , New<v8::String>( "getPrefix" ).ToLocalChecked()
       , New<v8::FunctionTemplate>( getPrefix )->GetFunction()
       );

  Set( target
       , New<v8::String>( "setAddress" ).ToLocalChecked()
       , New<v8::FunctionTemplate>( setAddress )->GetFunction()
       );

  Set( target
       , New<v8::String>( "getAddress" ).ToLocalChecked()
       , New<v8::FunctionTemplate>( getAddress )->GetFunction()
       );

  Set( target
       , New<v8::String>( "getCustomDiff" ).ToLocalChecked()
       , New<v8::FunctionTemplate>( getCustomDiff )->GetFunction()
       );

  Set( target
       , New<v8::String>( "setDiff" ).ToLocalChecked()
       , New<v8::FunctionTemplate>( setDiff )->GetFunction()
       );

  Set( target
       , New<v8::String>( "getDiff" ).ToLocalChecked()
       , New<v8::FunctionTemplate>( getDiff )->GetFunction()
       );

  Set( target
       , New<v8::String>( "getPoolUrl" ).ToLocalChecked()
       , New<v8::FunctionTemplate>( getPoolUrl )->GetFunction()
       );

  Set( target
       , New<v8::String>( "resetHashCounter" ).ToLocalChecked()
       , New<v8::FunctionTemplate>( resetHashCounter )->GetFunction()
       );

  Set( target
       , New<v8::String>( "incSolCount" ).ToLocalChecked()
       , New<v8::FunctionTemplate>( incSolCount )->GetFunction()
       );

  Set( target
       , New<v8::String>( "getSolution" ).ToLocalChecked()
       , New<v8::FunctionTemplate>( getSolution )->GetFunction()
       );

  Set( target
       , New<v8::String>( "log" ).ToLocalChecked()
       , New<v8::FunctionTemplate>( log )->GetFunction()
       );

  Set( target
       , New<v8::String>( "getTokenName" ).ToLocalChecked()
       , New<v8::FunctionTemplate>( getTokenName )->GetFunction()
       );

  Set( target
       , New<v8::String>( "getSubmitStale" ).ToLocalChecked()
       , New<v8::FunctionTemplate>( getSubmitStale )->GetFunction()
       );

  Set( target
       , New<v8::String>( "keccak256" ).ToLocalChecked()
       , New<v8::FunctionTemplate>( keccak256 )->GetFunction()
       );
  hybridminer = new HybridMiner;

  node::AtExit( cleanup, hybridminer );
}

NODE_MODULE( node_hybridminer, miner::Init )
