/** @file vtkCUDAObject.cxx
*
*  @brief An abstract class which uses CUDA
*
*  @author John Stuart Haberl Baxter (Dr. Peter's Lab at Robarts Research Institute)
*  @note First documented on June 12, 2012
*
*  @note Interacts primarily with the vtkCUDADeviceManager
*/

#include "vtkCUDAObject.h"
#include "cuda_runtime_api.h"
#include "vtkCUDADeviceManager.h"

// VTK includes
#include <vtkObjectFactory.h>

void errorOut(vtkCUDAObject* self, const char* message)
  {
  if (vtkObject::GetGlobalWarningDisplay())                    
    {                                                          
    vtkOStreamWrapper::EndlType endl;                          
    vtkOStreamWrapper::UseEndl(endl);                          
    vtkOStrStreamWrapper vtkmsg;                               
    vtkmsg << "ERROR: In " __FILE__ ", line " << __LINE__      
      << "\n" << "vtkCUDAObject" << " (" << self     
      << "): " << message << "\n\n";                                          
    vtkmsg.rdbuf()->freeze(0); vtkObject::BreakOnError();      
    }                                                          
  }

vtkCUDAObject::vtkCUDAObject()
  {
  this->DeviceManager = vtkCUDADeviceManager::Singleton();
  this->DeviceStream = 0;
  this->DeviceNumber = 0;
  bool result = this->DeviceManager->GetDevice(this, this->DeviceNumber);
  if(result)
    {
    errorOut(this,"Device selected cannot be retrieved.");
    this->DeviceNumber = -1;
    return;
    }
  result = this->DeviceManager->GetStream(this, &(this->DeviceStream), this->DeviceNumber );
  if(result)
    {
    errorOut(this,"Device selected cannot be retrieved.");
    this->DeviceManager->ReturnDevice(this, this->DeviceNumber );
    this->DeviceNumber = -1;
    return;
    }
  }

vtkCUDAObject::~vtkCUDAObject()
  {
  //synchronize remainder of stream and return control of the device
  this->CallSyncThreads();
  this->DeviceManager->ReturnDevice( this, this->DeviceNumber );
  }

void vtkCUDAObject::SetDevice( int d, int withData )
  {
  int numberOfDevices = this->DeviceManager->GetNumberOfDevices();

  if( d < 0 || d >= numberOfDevices )
    {
    errorOut(this,"Device selected does not exist.");
    return;
    }

  //set up a purely new device
  if( this->DeviceNumber == -1 )
    {
    this->DeviceNumber = d;
    bool result = this->DeviceManager->GetDevice(this, this->DeviceNumber);
    if(result)
      {
      errorOut(this,"Device selected cannot be retrieved.");
      this->DeviceNumber = -1;
      return;
      }
    result = this->DeviceManager->GetStream(this, &(this->DeviceStream), this->DeviceNumber );
    if(result)
      {
      errorOut(this,"Device selected cannot be retrieved.");
      this->DeviceManager->ReturnDevice(this, this->DeviceNumber );
      this->DeviceNumber = -1;
      return;
      }
    this->Reinitialize(withData);

    //if we are currently using that device, don't change anything
    }
  else if(this->DeviceNumber == d)
    {
    return;

    //finish all device business and set up a new device
    }
  else
    {
    this->Deinitialize(withData);
    this->DeviceManager->ReturnStream(this, this->DeviceStream, this->DeviceNumber );
    this->DeviceStream = 0;
    this->DeviceManager->ReturnDevice(this, this->DeviceNumber );
    this->DeviceNumber = d;
    bool result = this->DeviceManager->GetDevice(this, this->DeviceNumber);
    if(result)
      {
      errorOut(this,"Device selected cannot be retrieved.");
      this->DeviceNumber = -1;
      return;
      }
    result = this->DeviceManager->GetStream(this, &(this->DeviceStream), this->DeviceNumber );
    if(result)
      {
      errorOut(this,"Device selected cannot be retrieved.");
      this->DeviceManager->ReturnDevice(this, this->DeviceNumber );
      this->DeviceNumber = -1;
      return;
      }
    this->Reinitialize(withData);
    }
  }

void vtkCUDAObject::ReserveGPU( )
  {
  if( this->DeviceNumber == -1 )
    {
    errorOut(this,"No device set selected does not exist.");
    return;
    }
  if(this->DeviceManager->ReserveGPU(this->DeviceStream))
    {
    errorOut(this,"Error REserving GPU");
    return;
    }
  }

void vtkCUDAObject::CallSyncThreads( )
  {
  if( this->DeviceNumber == -1 )
    {
    errorOut(this,"No device set selected does not exist.");
    return;
    }
  if(this->DeviceManager->SynchronizeStream(this->DeviceStream))
    {
    errorOut(this,"Error Synchronizing Streams");
    return;
    }
  }

cudaStream_t* vtkCUDAObject::GetStream( )
  {
  return this->DeviceStream;
  }

void vtkCUDAObject::ReplicateObject( vtkCUDAObject* object, int withData )
  {
  int oldDeviceNumber = this->DeviceNumber;
  this->SetDevice( object->DeviceNumber, withData );
  if( this->DeviceStream != object->DeviceStream )
    {
    this->CallSyncThreads();
    this->DeviceManager->ReturnStream(this, this->DeviceStream, oldDeviceNumber);
    this->DeviceStream = 0;
    this->DeviceStream = object->DeviceStream;
    this->DeviceManager->GetStream( this, &(object->DeviceStream), object->DeviceNumber );
    }
  }