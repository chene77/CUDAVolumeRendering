/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// CUDA Volume Rendering includes
#include "vtkSlicerCUDAVolumeRenderingLogic.h"
#include "vtkMRMLCUDARayCastVolumeRenderingDisplayNode.h"

// VTK includes
#include <vtkNew.h>

// STD includes
#include <cassert>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerCUDAVolumeRenderingLogic);

//----------------------------------------------------------------------------
vtkSlicerCUDAVolumeRenderingLogic::vtkSlicerCUDAVolumeRenderingLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerCUDAVolumeRenderingLogic::~vtkSlicerCUDAVolumeRenderingLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerCUDAVolumeRenderingLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerCUDAVolumeRenderingLogic::RegisterNodes()
{
  vtkNew<vtkMRMLCUDARayCastVolumeRenderingDisplayNode> displayNode;
  this->GetMRMLScene()->RegisterNodeClass(displayNode.GetPointer());
}
