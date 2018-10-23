#include "PC3DHdr.h"
#include "camera.h"
#include "Scene.h"

CCamera::CCamera()
{
  m_MovingToTarget = FALSE;

}

void CCamera::Init()
{
  //set the default camera at 0,0,-1 to look at 0,0,0.
  vector  from(0.0f, 160.0f, -320.0f);
  vector  dir(0.0f, -160.0f, 320.0f);
  vector  up(0.0f, 1.0f, 0.0f);

  //vector  from(0.0f, 0.0f, -1.0f);
  //vector  dir(0.0f, 0.0f, 1.0f);
  //vector  up(0.0f, 1.0f, 0.0f);
  vector  scale(1.0f, 1.0f, 1.0f);

  //Initialise lookatpoint, which will be used by orbit and other similar function

  m_LookAt.SetPosition( &from );
  m_LookAt.SetOrientation( &dir, &up );
  m_LookAt.SetScale( &scale );

  //Initialise the camera also...
  m_Camera->SetPosition( &from );
  m_Camera->SetOrientation( &dir, &up );
  m_Camera->SetScale( &scale );
}

CCamera::CCamera( FrameType * Master )
{
  m_Camera = Master;
}

CCamera::~CCamera()
{
  /*
  m_Camera->detachChild( m_LookAtPoint );
  delete m_LookAtPoint;
  */
}

//Camera position
//these function move everything, even the lookat spot
void CCamera::Foward( float Distance )
{
  vector Dir;
  m_Camera->GetDirection(&Dir);
  Dir = Normalize(Dir);
  vector Pos, NewPos;
  m_Camera->GetPosition(&Pos);
  NewPos = Pos;
  NewPos += (Dir * Distance);
  m_Camera->SetPosition(&NewPos);
}

void CCamera::Backward( float Distance )
{
  vector Dir;
  m_Camera->GetDirection(&Dir);
  Dir = Normalize( Dir );
  vector Pos, NewPos;
  m_Camera->GetPosition(&Pos);
  NewPos = Pos;
  NewPos -= (Dir * Distance);
  m_Camera->SetPosition(&NewPos);
}

void CCamera::Up( float Distance )
{
  vector Dir;
  m_Camera->GetUp(&Dir);
  Dir = Normalize( Dir );
  vector Pos, NewPos;
  m_Camera->GetPosition(&Pos);
  NewPos = Pos;
  NewPos += (Dir * Distance);
  m_Camera->SetPosition(&NewPos);
}

void CCamera::Down( float Distance )
{
  vector Dir;
  m_Camera->GetUp(&Dir);
  Dir = Normalize( Dir );
  vector Pos, NewPos;
  m_Camera->GetPosition(&Pos);
  NewPos = Pos;
  NewPos -= (Dir * Distance);
  m_Camera->SetPosition(&NewPos);
}

void CCamera::Left( float Distance )
{
  vector  Dir,Up;
  vector Pos, NewPos, PerpVect;

  m_Camera->GetDirection(&Dir);
  m_Camera->GetUp(&Up);
  Dir = Normalize( Dir );
  Up = Normalize( Up );

  m_Camera->GetPosition(&Pos);

  PerpVect = CrossProduct(Up, Dir);

  NewPos = Pos;
  NewPos -= (PerpVect * Distance); 
  m_Camera->SetPosition(&NewPos);
}

void CCamera::Right( float Distance )
{
  vector  Dir,Up;
  vector Pos, NewPos, PerpVect;

  m_Camera->GetDirection(&Dir);
  m_Camera->GetUp(&Up);
  Dir = Normalize( Dir );
  Up = Normalize( Up );

  m_Camera->GetPosition(&Pos);

  PerpVect = CrossProduct(Up, Dir);

  NewPos = Pos;
  NewPos += (PerpVect * Distance); 
  m_Camera->SetPosition(&NewPos);
}

//Camera position around the lookat spot
//doesn't change the lookat spot
void CCamera::OrbitFoward();
void CCamera::OrbitBackward();
void CCamera::OrbitUp( float RadAngle )
{
  if((RadAngle>0.001)||(RadAngle<-0.001))
  {
    vector CamPos;
    vector LookAtPos;
    vector CamToObjOffset;
    vector NewCamToObjOffset;
    vector RotationAxis;
    vector ObjUp(0, 1, 0), ObjDir( 0, 0, 1);
    vector PerpVect;
    //vector CamUp(0, 0, 0);

    m_Camera->GetPosition(&CamPos);
    //m_Camera->GetUp(&CamUp);
    m_LookAt.GetPosition(&LookAtPos);
    m_LookAt.GetOrientation(&ObjDir,&ObjUp);

    PerpVect = CrossProduct(ObjUp, ObjDir);

    CamToObjOffset = LookAtPos - CamPos;

    Matrix RotMat = Matrix::RotateAxisMatrix(PerpVect, RadAngle);

    NewCamToObjOffset = RotMat.TransVector(CamToObjOffset);
    //CamUp = TransformVector( CamUp, RotMat );

    NewCamToObjOffset  -= CamToObjOffset;
    CamPos += NewCamToObjOffset;

    m_Camera->SetPosition(&CamPos);
    CamToObjOffset = Normalize( CamToObjOffset );
    m_Camera->SetDirection( &CamToObjOffset );
    //m_Camera->SetUp(&CamUp);
  }
}


void CCamera::OrbitDown()
{
  /*
  vector Up;
  vector NewDir;
  vector Dir;
  m_Camera->getUp(&Up);
  NormaliseVector( &Up );
  m_Camera->getDirection(&Dir);
  NewDir  = (Dir);
  NewDir  -= (Up * Distance);
  m_Camera->setPosition(&NewDir);
  NewDir = NewDir * -1;
  m_Camera->setDirection(&NewDir);
  */
}

void CCamera::OrbitLeft();
void CCamera::OrbitRight();
void CCamera::Goto(FrameType *LookTo, BOOL LookClose )
{
  vector  from(0.0f, 0.0f, -1.0f);
  vector  dir(0.0f, 0.0f, 1.0f);
  vector  up(0.0f, 1.0f, 0.0f);

  vector  VU(0.0f, 0.0f, -1.0f);
  vector  VD(0.0f, 0.0f, 1.0f);
  vector  VP(0.0f, 1.0f, 0.0f);

  Matrix WMatrix = *(LookTo->GetWorldMatrix());

  WMatrix.TransformVector(VU);
  WMatrix.TransformVector(VD);
  WMatrix.TransformVector(VP);

  VU = Normalize(VU);
  VD = Normalize(VD);

  m_LookAt.SetUp(&VU);
  m_LookAt.SetDirection(&VD);
  m_LookAt.SetPosition(&VP);

  vector CamPos;
  vector LookAtPos;

  m_Camera->GetPosition(&CamPos);
  m_LookAt.GetPosition(&LookAtPos);

  dir = LookAtPos - CamPos;

  if(LookClose)
  {
    //Change the position of the camera to have a closer look at the object.
    //Going foward on the direction vector by a certain amount that would
    //make the object be fully visible.
    Box extent3D;

    if(LookTo->GetExtent3D(NULL, &extent3D))
    {
      float Max;

      //Find the largest side.
      
      //Max  = max(fabs((double)(extent3D.minx - extent3D.maxx)),fabs((double)(extent3D.miny - extent3D.maxy)));
      //Max  = max(Max,fabs((double)(extent3D.minz - extent3D.maxz)));

      //TODO/FIXME
      //HELP, extent3D doesn't work properly so Max is hardcoded here, 
      Max  = 150.0f;
      float Distance = (float)((Max/2)/tan(VIEWING_ANGLE/2));
      float factor = Distance/Magnitude(dir);

      vector Movement = dir * factor;
      Movement = dir - Movement;
      CamPos += Movement;
      m_Camera->SetPosition( &CamPos ); 
    }
  }
  m_Camera->SetOrientation( &dir, &up );
  
}

//Camera lookat spot
//these function move everything, even the lookat spot
void CCamera::Roll(float RadAngle)
{
  vector Dir,Up;
  m_Camera->GetDirection(&Dir);
  m_Camera->GetUp(&Up);
  Dir = Normalize( Dir );
  Up = Normalize( Up );

  Matrix RotMat = Matrix::RotateAxisMatrix(Dir, RadAngle);

  RotMat.TransformVector(Up);

  m_Camera->SetUp( &Up );

}

void CCamera::Pitch(float RadAngle)
{
  vector  Dir,Up,PerpVect;

  m_Camera->GetDirection(&Dir);
  m_Camera->GetUp(&Up);
  Dir = Normalize( Dir );
  Up = Normalize( Up );

  PerpVect = CrossProduct(Up, Dir);

  Matrix RotMat = Matrix::RotateAxisMatrix( PerpVect, RadAngle );

  RotMat.TransformVector(Dir);
  RotMat.TransformVector(Up);

  m_Camera->SetDirection( &Dir );
  m_Camera->SetUp( &Up );
}

void CCamera::Yaw(float RadAngle)
{
  vector  Dir,Up;
  m_Camera->GetDirection(&Dir);
  m_Camera->GetUp(&Up);
  Dir = Normalize( Dir );
  Up = Normalize( Up );

  Matrix RotMat = Matrix::RotateAxisMatrix( Up, RadAngle );

  RotMat.TransformVector(Dir);

  m_Camera->SetDirection( &Dir );

}

//other
//this is for centering the camera at a certain object(frame).
void CCamera::ResetAll()
{
  vector  from(0.0f, 0.0f, -1.0f);
  vector  dir(0.0f, 0.0f, 1.0f);
  vector  up(0.0f, 1.0f, 0.0f);
  vector  scale(1.0f, 1.0f, 1.0f);

  m_Camera->SetPosition( &from );
  m_Camera->SetOrientation( &dir, &up );
  m_Camera->SetScale( &scale );
}

void CCamera::setCamera(FrameType *cam)
{
  m_Camera = cam;
}

FrameType* CCamera::getCamera()
{
  return m_Camera;
}

void CCamera::SetCameraTarget( FrameType * NewTarget, int NbFrameToReachTarget )
{
  m_MovingToTarget = TRUE;

  Matrix mAnim = *(NewTarget->GetWorldMatrix());
  
  // recreate the frame from the given raw matrix
  D3DVECTOR Pos = mAnim.TransVector(D3DVECTOR(0, 0, 0));
  D3DVECTOR Dir = mAnim.TransVector(D3DVECTOR(0, 0, 1.0f)) - Pos;
  D3DVECTOR Up = mAnim.TransVector(D3DVECTOR(0, 1.0f, 0)) - Pos;

  //NewTarget->getPosition( &Pos );
  m_Target.SetPosition( &Pos );

  //NewTarget->getOrientation( &Up, &Dir );
  m_Target.SetOrientation( &Dir, &Up );

  m_Camera->GetPosition( &Pos );
  m_Source.SetPosition( &Pos );

  m_Camera->GetOrientation( &Dir, &Up );
  m_Source.SetOrientation( &Dir, &Up );

  m_NbFrameToTarget = NbFrameToReachTarget;
  m_CurrentFrameToTarget = 0;
}

void CCamera::UpdateCameraSmooth()
{
  if(!m_MovingToTarget)
    return;

  vector Position;
  vector Up, Dir;

  if(m_CurrentFrameToTarget<=m_NbFrameToTarget)
  {
    vector PositionSource;
    vector PositionTarget;
    vector NewPosition;

    vector UpSource, DirSource;
    vector UpTarget, DirTarget;
    vector NewUp, NewDir;

    m_CurrentFrameToTarget ++;

    //here interpolate new position and orientation using some math.
    //using sin for now....
    //TODO need optimisation...
    //float Percentage = (float)( ((float)m_CurrentFrameToTarget) / ((float)m_NbFrameToTarget-1)  );
    //Percentage = (float) sin( (Percentage*90.0f)/100.0f * (PIFLOAT/2) );
    float Percentage = (float)( ((float)m_CurrentFrameToTarget) / ((float)m_NbFrameToTarget)  ); // 1 at final frame
    Percentage = (float) sin( Percentage * PIFLOAT / 2 ); // at final frame, sin(PIFLOAT/2) = sin(90degrees)

    //new position calculations
    m_Source.GetPosition( &PositionSource );
    m_Target.GetPosition( &PositionTarget );

    m_Source.GetOrientation( &DirSource, &UpSource );
    m_Target.GetOrientation( &DirTarget, &UpTarget );

    NewPosition = (PositionTarget - PositionSource) * Percentage;
    NewPosition += PositionSource;

    m_Camera->SetPosition( &NewPosition );

    //New Orientation calculations
    NewDir = (DirTarget - DirSource) * Percentage;
    NewDir += DirSource;
    NewUp = (UpTarget - UpSource) * Percentage;
    NewUp += UpSource;
    m_Camera->SetOrientation( &NewDir, &NewUp );
  }
  else
  {
    m_MovingToTarget = FALSE;//we are done moving......
    m_Target.GetPosition( &Position );
    m_Camera->SetPosition( &Position );
    m_Target.GetOrientation( &Dir, &Up );
    m_Camera->SetOrientation( &Dir, &Up );
  }
}


void CCamera::UpdateCameraSmoothStartAndStop()
{
  if(!m_MovingToTarget)
    return;

  vector Position;
  vector Up, Dir;

  if(m_CurrentFrameToTarget<=m_NbFrameToTarget)
  {
    vector PositionSource;
    vector PositionTarget;
    vector NewPosition;

    vector UpSource, DirSource;
    vector UpTarget, DirTarget;
    vector NewUp, NewDir;

    m_CurrentFrameToTarget ++;

    //here interpolate new position and orientation using some math.
    //using sin for now....
    //TODO need optimisation...
    float Percentage = (float)( ((float)m_CurrentFrameToTarget) / ((float)m_NbFrameToTarget)  );
    if( Percentage < 0.5f ) // Accelerate to the center, arrive at half time.
      Percentage = 0.5f - 0.5f*(float) cos( Percentage * (PIFLOAT) );
    else                    // Decelerate to destination, arrive in last frame.
      Percentage = 0.5f + 0.5f*(float) sin( (Percentage - 0.5f) * (PIFLOAT) );

    //new position calculations
    m_Source.GetPosition( &PositionSource );
    m_Target.GetPosition( &PositionTarget );

    m_Source.GetOrientation( &DirSource, &UpSource );
    m_Target.GetOrientation( &DirTarget, &UpTarget );

    NewPosition = (PositionTarget - PositionSource) * Percentage;
    NewPosition += PositionSource;

    m_Camera->SetPosition( &NewPosition );

    //New Orientation calculations
    NewDir = (DirTarget - DirSource) * Percentage;
    NewDir += DirSource;
    NewUp = (UpTarget - UpSource) * Percentage;
    NewUp += UpSource;
    m_Camera->SetOrientation( &NewDir, &NewUp );
  }
  else
  {
    m_MovingToTarget = FALSE;//we are done moving......
    m_Target.GetPosition( &Position );
    m_Camera->SetPosition( &Position );
    m_Target.GetOrientation( &Dir, &Up );
    m_Camera->SetOrientation( &Dir, &Up );
  }
}

//Steph:
//This makes the camera orbit around a point in 3D space
//center is the point of orbit, and angle is the angle in radians
void CCamera::orbitAroundX(vector center, float angle)
{
  vector  position, direction, up, perp;
  Matrix  rotMat, calcMat, destMat;

  //get camera vectors
  m_Camera->GetPosition(&position);
  m_Camera->GetDirection(&direction);
  m_Camera->GetUp(&up);
  perp = CrossProduct(up, direction);

  //translate to origin
  position -= center;

  //calculate new position
  rotMat = Matrix::RotateAxisMatrix(perp, angle);
  calcMat.Identity();
  calcMat._31 = position.x;
  calcMat._32 = position.y;
  calcMat._33 = position.z;
  destMat = calcMat * rotMat;
  position = vector(destMat._31, destMat._32, destMat._33);

  //calculate new direction
  calcMat.Identity();
  calcMat._31 = direction.x;
  calcMat._32 = direction.y;
  calcMat._33 = direction.z;
  destMat = calcMat * rotMat;
  direction = vector(destMat._31, destMat._32, destMat._33);

  //calculate new up
  calcMat.Identity();
  calcMat._31 = up.x;
  calcMat._32 = up.y;
  calcMat._33 = up.z;
  destMat = calcMat * rotMat;
  up = vector(destMat._31, destMat._32, destMat._33);

  //translate to real position
  position += center;

  //set new camera vectors
  m_Camera->SetPosition(&position);
  m_Camera->SetDirection(&direction);
  m_Camera->SetUp(&up);
}

//Steph:
//This makes the camera orbit around a point in 3D space
//center is the point of orbit, and angle is the angle in radians
void CCamera::orbitAroundY(vector center, float angle)
{
  vector  position, direction, up;
  Matrix  rotMat, calcMat, destMat;

  //get camera vectors
  m_Camera->GetPosition(&position);
  m_Camera->GetDirection(&direction);
  m_Camera->GetUp(&up);

  //translate to origin
  position -= center;

  //calculate new position
  rotMat = Matrix::RotateAxisMatrix(up, angle);
  calcMat.Identity();
  calcMat._31 = position.x;
  calcMat._32 = position.y;
  calcMat._33 = position.z;
  destMat = calcMat * rotMat;
  position = vector(destMat._31, destMat._32, destMat._33);

  //calculate new direction
  calcMat.Identity();
  calcMat._31 = direction.x;
  calcMat._32 = direction.y;
  calcMat._33 = direction.z;
  destMat = calcMat * rotMat;
  direction = vector(destMat._31, destMat._32, destMat._33);

  //translate to real position
  position += center;

  //set new camera vectors
  m_Camera->SetPosition(&position);
  m_Camera->SetDirection(&direction);
}

//________________________________________________________________________________
//Originaly written by Stephane Lebrun
//added to this file for convenience.   
//
//Note: that the function receives a frame.
//i could have apply the change directly to the camera
//but this way, the function can be used for other frames.
//for that function to work, the frame must be in not override matrix mode. OverwriteMatrix = false
// -TOC
void CCamera::PitchYawRoll(FrameType* m, float fPitch, float fYaw, float fRoll)
{
  Matrix  mtrx(Matrix::IDENTITY);
  vector  vec;

  if (fPitch) mtrx.RotateX(fPitch);
  if (fYaw)   mtrx.RotateY(fYaw);
  if (fRoll)  mtrx.RotateZ(fRoll);

  //convert to direction and up vectors then update frame
//  mat = MatrixMult(MatrixMult(UTILS_RotateZMatrix(RadRoll),
//                              UTILS_RotateXMatrix(RadPitch)),
//                   UTILS_RotateYMatrix(RadYaw));
  
  vec.x = mtrx._31; vec.y = mtrx._32; vec.z = mtrx._33;
  m->SetDirection(&vec);
  vec.x = mtrx._21; vec.y = mtrx._22; vec.z = mtrx._23;
  m->SetUp(&vec);
}

