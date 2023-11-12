#ifndef CONSTRAINTELEMH
#define CONSTRAINTELEMH

class CBtObject;
class CBone;
class btConeTwistConstraint;
class btGeneric6DofSpringConstraint;

typedef struct tag_constraintelem
{
	CBtObject* childbto;
	CBone* centerbone;
	btGeneric6DofSpringConstraint* constraint;
} CONSTRAINTELEM;


#endif
