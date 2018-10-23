#ifndef MATMAN_H
#define MATMAN_H


// .......... objects that creates, manages, and destroys materials
class materialManager {

public:
      materialManager();
      ~materialManager();
	
	material *createMaterial(PMATERIALDATA mi);
	BOOL	destroyMaterial(material *pMat);

	getMaterial();

	BOOL initMeshMaterial( mesh *m );
	BOOL setMeshMaterial(PMATERIALDATA mi, mesh *m );
};

#endif



