#include <fstream>
#include "FluxusBinding.h"

FluxusMain     *FluxusBinding::Fluxus=NULL;
AudioCollector *FluxusBinding::Audio=NULL;
TurtleBuilder   FluxusBinding::turtle;
string          FluxusBinding::CallbackString;
long            FluxusBinding::FrameCount=0;
PolyPrimitive*  FluxusBinding::StaticCube=NULL;
PolyPrimitive*  FluxusBinding::StaticPlane=NULL;
PolyPrimitive*  FluxusBinding::StaticSphere=NULL;
PolyPrimitive*  FluxusBinding::StaticCylinder=NULL;
set<int>        FluxusBinding::m_KeySet;

FluxusBinding::FluxusBinding(int w, int h) 
{
	StaticCube = new PolyPrimitive(PolyPrimitive::QUADS);
    MakeCube(StaticCube);
    StaticCube->Finalise();

	StaticPlane = new PolyPrimitive(PolyPrimitive::QUADS);
    MakePlane(StaticCube);
    StaticPlane->Finalise();

	StaticSphere = new PolyPrimitive(PolyPrimitive::TRILIST);
    MakeSphere(StaticSphere,1,5,10);
    StaticSphere->Finalise();

	StaticCylinder = new PolyPrimitive(PolyPrimitive::TRILIST);
    MakeCylinder(StaticCylinder,1,1,5,10);
    StaticCylinder->Finalise();
	
	Audio = new AudioCollector(AUDIO_BUFFER_SIZE);
	Fluxus = new FluxusMain(w,h);
}

FluxusBinding::~FluxusBinding()
{
	delete Audio;
	delete Fluxus;
}

SCM FluxusBinding::build_cube()
{
	PolyPrimitive *BoxPrim = new PolyPrimitive(PolyPrimitive::QUADS);
    MakeCube(BoxPrim);
    BoxPrim->Finalise();
    return gh_double2scm(Fluxus->GetRenderer()->AddPrimitive(BoxPrim));
}

SCM FluxusBinding::build_plane()
{
	PolyPrimitive *PlanePrim = new PolyPrimitive(PolyPrimitive::QUADS);
    MakePlane(PlanePrim);
    PlanePrim->Finalise();    	
    return gh_double2scm(Fluxus->GetRenderer()->AddPrimitive(PlanePrim));
}


SCM FluxusBinding::build_cylinder(SCM s_hsegments, SCM s_rsegments)
{
	SCM_ASSERT(SCM_NUMBERP(s_hsegments), s_hsegments, SCM_ARG1, "h segments");
	SCM_ASSERT(SCM_NUMBERP(s_rsegments), s_rsegments, SCM_ARG2, "r segments");
	double hsegments;
	hsegments=gh_scm2double(s_hsegments);
    double rsegments;
	rsegments=gh_scm2double(s_rsegments);	

	PolyPrimitive *CylPrim = new PolyPrimitive(PolyPrimitive::TRILIST);
    MakeCylinder(CylPrim, 1, 1, (int)hsegments, (int)rsegments);
    CylPrim->Finalise();    	

    return gh_double2scm(Fluxus->GetRenderer()->AddPrimitive(CylPrim));
}

SCM FluxusBinding::build_line(SCM start, SCM swidth, SCM end, SCM ewidth)
{
	SCM_ASSERT(SCM_VECTORP(start), start, SCM_ARG1, "start");
	SCM_ASSERT(SCM_NUMBERP(swidth), swidth, SCM_ARG2, "start width");
	SCM_ASSERT(SCM_VECTORP(end), end, SCM_ARG3, "end");
	SCM_ASSERT(SCM_NUMBERP(ewidth), ewidth, SCM_ARG4, "end width");
	float s[3],e[3];
	gh_scm2floats(start,s);
	gh_scm2floats(end,e);
	
	LinePrimitive *LinePrim = new LinePrimitive;
	LinePrim->SetStart(dVertex(dVector(s[0],s[1],s[2]),dVector(0,1,0)),gh_scm2double(swidth));
	LinePrim->SetEnd(dVertex(dVector(e[0],e[1],e[2]),dVector(0,1,0)),gh_scm2double(ewidth));
	
	return gh_double2scm(Fluxus->GetRenderer()->AddPrimitive(LinePrim));
}

SCM FluxusBinding::draw_cube()
{    	
    Fluxus->GetRenderer()->RenderPrimitive(StaticCube);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::draw_plane()
{    	
    Fluxus->GetRenderer()->RenderPrimitive(StaticPlane);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::draw_sphere()
{    	
    Fluxus->GetRenderer()->RenderPrimitive(StaticSphere);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::draw_cylinder()
{    	
    Fluxus->GetRenderer()->RenderPrimitive(StaticCylinder);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::build_sphere(SCM s_hsegments, SCM s_rsegments)
{
	SCM_ASSERT(SCM_NUMBERP(s_hsegments), s_hsegments, SCM_ARG1, "h segments");
	SCM_ASSERT(SCM_NUMBERP(s_rsegments), s_rsegments, SCM_ARG2, "r segments");
	double hsegments;
	hsegments=gh_scm2double(s_hsegments);
    double rsegments;
	rsegments=gh_scm2double(s_rsegments);	
	
	PolyPrimitive *SphPrim = new PolyPrimitive(PolyPrimitive::TRILIST);
    MakeSphere(SphPrim, 1, (int)hsegments, (int)rsegments);
    SphPrim->Finalise();    	
    return gh_double2scm(Fluxus->GetRenderer()->AddPrimitive(SphPrim));
}

SCM FluxusBinding::key_pressed(SCM s_key)
{
	SCM_ASSERT(SCM_STRINGP(s_key), s_key, SCM_ARG1, "key");
	char *key=0;
	size_t size=0;
	key=gh_scm2newstr(s_key,&size);
	double pressed;
	if (m_KeySet.find(key[0])!=m_KeySet.end()) pressed=1;
	else pressed=0;
    free(key);
    return gh_bool2scm((int)pressed);
}

SCM FluxusBinding::show_axis(SCM s_id)
{
	SCM_ASSERT(SCM_NUMBERP(s_id), s_id, SCM_ARG1, "texture id");	
    Fluxus->GetRenderer()->ShowAxis(gh_scm2int(s_id));
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::show_fps(SCM s_id)
{
	SCM_ASSERT(SCM_NUMBERP(s_id), s_id, SCM_ARG1, "texture id");	
    Fluxus->GetRenderer()->SetFPSDisplay(gh_scm2int(s_id));
    return SCM_UNSPECIFIED;
}
SCM FluxusBinding::make_light(SCM cam)
{
	SCM_ASSERT(SCM_NUMBERP(cam), cam, SCM_ARG1, "camera locked");
	Light *l=new Light;
	l->SetCameraLock((bool)gh_scm2double(cam));
	return gh_double2scm(Fluxus->GetRenderer()->AddLight(l));
}

SCM FluxusBinding::clear_lights()
{
	Fluxus->GetRenderer()->ClearLights();
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::light_ambient(SCM id, SCM v)
{
	SCM_ASSERT(SCM_NUMBERP(id), id, SCM_ARG1, "light id");	
	SCM_ASSERT(SCM_VECTORP(v), v, SCM_ARG2, "ambient");
	float vec[3];
	gh_scm2floats(v,vec);
	Fluxus->GetRenderer()->GetLight((int)gh_scm2double(id))->SetAmbient(dColour(vec[0],vec[1],vec[2]));
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::light_diffuse(SCM id, SCM v)
{
	SCM_ASSERT(SCM_NUMBERP(id), id, SCM_ARG1, "light id");	
	SCM_ASSERT(SCM_VECTORP(v), v, SCM_ARG2, "diffuse");
	float vec[3];
	gh_scm2floats(v,vec);
	Fluxus->GetRenderer()->GetLight((int)gh_scm2double(id))->SetDiffuse(dColour(vec[0],vec[1],vec[2]));
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::light_specular(SCM id, SCM v)
{
	SCM_ASSERT(SCM_NUMBERP(id), id, SCM_ARG1, "light id");	
	SCM_ASSERT(SCM_VECTORP(v), v, SCM_ARG2, "specular");
	float vec[3];
	gh_scm2floats(v,vec);
	Fluxus->GetRenderer()->GetLight((int)gh_scm2double(id))->SetSpecular(dColour(vec[0],vec[1],vec[2]));
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::light_position(SCM id, SCM v)
{
	SCM_ASSERT(SCM_NUMBERP(id), id, SCM_ARG1, "light id");	
	SCM_ASSERT(SCM_VECTORP(v), v, SCM_ARG2, "position");
	float vec[3];
	gh_scm2floats(v,vec);
	Fluxus->GetRenderer()->GetLight((int)gh_scm2double(id))->SetPosition(dVector(vec[0],vec[1],vec[2]));
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::lock_camera(SCM s_ob)
{
	SCM_ASSERT(SCM_NUMBERP(s_ob), s_ob, SCM_ARG1, "primitive");
	int ob=0;
	ob=(int)gh_scm2double(s_ob);	
    Fluxus->GetRenderer()->LockCamera(ob);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::destroy(SCM s_name)
{
	SCM_ASSERT(SCM_NUMBERP(s_name), s_name, SCM_ARG1, "name");
	int name=0;
	name=(int)gh_scm2double(s_name);	
	
	Primitive *p=Fluxus->GetRenderer()->GetPrimitive(name);
	if (p)
	{
    	if (p->IsPhysicalHint())
    	{
    		Fluxus->GetPhysics()->Free(name);
    	}
    	Fluxus->GetRenderer()->RemovePrimitive(name);
    }

    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::clear()
{
	Fluxus->GetRenderer()->Clear();
	Fluxus->GetPhysics()->Clear();
	Fluxus->ClearLifeforms();
	CallbackString="";
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::grab(SCM s_id)
{
	SCM_ASSERT(SCM_NUMBERP(s_id), s_id, SCM_ARG1, "id");
	Fluxus->GetRenderer()->Grab((int)gh_scm2double(s_id));
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::ungrab()
{
	Fluxus->GetRenderer()->UnGrab();
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::apply(SCM s_id)
{
	SCM_ASSERT(SCM_NUMBERP(s_id), s_id, SCM_ARG1, "id");
	Fluxus->GetRenderer()->GetPrimitive((int)gh_scm2double(s_id))->ApplyTransform();
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::opacity(SCM s_opac)
{
    SCM_ASSERT(SCM_NUMBERP(s_opac), s_opac, SCM_ARG1, "opacity");
    Primitive *Grabbed=Fluxus->GetRenderer()->Grabbed();
    if (Grabbed) Grabbed->GetState()->Opacity=gh_scm2double(s_opac);
    else Fluxus->GetRenderer()->GetState()->Opacity=gh_scm2double(s_opac);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::shinyness(SCM s_opac)
{
    SCM_ASSERT(SCM_NUMBERP(s_opac), s_opac, SCM_ARG1, "shinyness");
    Primitive *Grabbed=Fluxus->GetRenderer()->Grabbed();
    if (Grabbed) Grabbed->GetState()->Shinyness=gh_scm2double(s_opac);
    else Fluxus->GetRenderer()->GetState()->Shinyness=gh_scm2double(s_opac);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::colour(SCM s_vec)
{
    SCM_ASSERT(SCM_VECTORP(s_vec), s_vec, SCM_ARG1, "colour");
	float col[3];
	gh_scm2floats(s_vec,col);
    Primitive *Grabbed=Fluxus->GetRenderer()->Grabbed();
    if (Grabbed) Grabbed->GetState()->Colour=dColour(col[0],col[1],col[2]);
    else Fluxus->GetRenderer()->GetState()->Colour=dColour(col[0],col[1],col[2]);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::specular(SCM s_vec)
{
    SCM_ASSERT(SCM_VECTORP(s_vec), s_vec, SCM_ARG1, "colour");
	float col[3];
	gh_scm2floats(s_vec,col);
    Primitive *Grabbed=Fluxus->GetRenderer()->Grabbed();
    if (Grabbed) Grabbed->GetState()->Specular=dColour(col[0],col[1],col[2]);
    else Fluxus->GetRenderer()->GetState()->Specular=dColour(col[0],col[1],col[2]);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::ambient(SCM s_vec)
{
    SCM_ASSERT(SCM_VECTORP(s_vec), s_vec, SCM_ARG1, "colour");
	float col[3];
	gh_scm2floats(s_vec,col);
    Primitive *Grabbed=Fluxus->GetRenderer()->Grabbed();
    if (Grabbed) Grabbed->GetState()->Ambient=dColour(col[0],col[1],col[2]);
    else Fluxus->GetRenderer()->GetState()->Ambient=dColour(col[0],col[1],col[2]);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::emissive(SCM s_vec)
{
    SCM_ASSERT(SCM_VECTORP(s_vec), s_vec, SCM_ARG1, "colour");
	float col[3];
	gh_scm2floats(s_vec,col);
    Primitive *Grabbed=Fluxus->GetRenderer()->Grabbed();
    if (Grabbed) Grabbed->GetState()->Emissive=dColour(col[0],col[1],col[2]);
    else Fluxus->GetRenderer()->GetState()->Emissive=dColour(col[0],col[1],col[2]);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::flux_identity()
{
    Primitive *Grabbed=Fluxus->GetRenderer()->Grabbed();
    if (Grabbed) Grabbed->GetState()->Transform.init();
    else Fluxus->GetRenderer()->GetState()->Transform.init();
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::translate(SCM s_vec)
{
    SCM_ASSERT(SCM_VECTORP(s_vec), s_vec, SCM_ARG1, "colour");
	float trans[3];
	gh_scm2floats(s_vec,trans);
    Primitive *Grabbed=Fluxus->GetRenderer()->Grabbed();
    if (Grabbed) Grabbed->GetState()->Transform.translate(trans[0],trans[1],trans[2]);
    else Fluxus->GetRenderer()->GetState()->Transform.translate(trans[0],trans[1],trans[2]);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::rotate(SCM s_vec)
{
    SCM_ASSERT(SCM_VECTORP(s_vec), s_vec, SCM_ARG1, "colour");
	float rot[0];
	gh_scm2floats(s_vec,rot);
    Primitive *Grabbed=Fluxus->GetRenderer()->Grabbed();
    if (Grabbed)
    {
    	Grabbed->GetState()->Transform.rotxyz(rot[0],rot[1],rot[2]);
    }
    else
    {
    	Fluxus->GetRenderer()->GetState()->Transform.rotxyz(rot[0],rot[1],rot[2]);
    }
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::scale(SCM s_vec)
{
    SCM_ASSERT(SCM_VECTORP(s_vec), s_vec, SCM_ARG1, "colour");
	float scale[3];
	gh_scm2floats(s_vec,scale);
    Primitive *Grabbed=Fluxus->GetRenderer()->Grabbed();
    if (Grabbed) Grabbed->GetState()->Transform.scale(scale[0],scale[1],scale[2]);
    else Fluxus->GetRenderer()->GetState()->Transform.scale(scale[0],scale[1],scale[2]);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::parent(SCM s_p)
{
    SCM_ASSERT(SCM_NUMBERP(s_p), s_p, SCM_ARG1, "parent");
    Fluxus->GetRenderer()->GetState()->Parent=(int)gh_scm2double(s_p);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::line_width(SCM s_p)
{
    SCM_ASSERT(SCM_NUMBERP(s_p), s_p, SCM_ARG1, "width");
    Primitive *Grabbed=Fluxus->GetRenderer()->Grabbed();
    if (Grabbed) Grabbed->GetState()->LineWidth=gh_scm2double(s_p);
    else Fluxus->GetRenderer()->GetState()->LineWidth=gh_scm2double(s_p);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::hint_solid()
{
    Primitive *Grabbed=Fluxus->GetRenderer()->Grabbed();
    if (Grabbed) Grabbed->GetState()->Hints|=HINT_SOLID;
    else Fluxus->GetRenderer()->GetState()->Hints|=HINT_SOLID;
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::hint_wire()
{
    Primitive *Grabbed=Fluxus->GetRenderer()->Grabbed();
    if (Grabbed) Grabbed->GetState()->Hints|=HINT_WIRE;
    else Fluxus->GetRenderer()->GetState()->Hints|=HINT_WIRE;
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::hint_normal()
{
    Primitive *Grabbed=Fluxus->GetRenderer()->Grabbed();
    if (Grabbed) Grabbed->GetState()->Hints|=HINT_NORMAL;
    else Fluxus->GetRenderer()->GetState()->Hints|=HINT_NORMAL;
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::hint_points()
{
    Primitive *Grabbed=Fluxus->GetRenderer()->Grabbed();
    if (Grabbed) Grabbed->GetState()->Hints|=HINT_POINTS;
    else Fluxus->GetRenderer()->GetState()->Hints|=HINT_POINTS;
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::hint_anti_alias()
{
    Primitive *Grabbed=Fluxus->GetRenderer()->Grabbed();
    if (Grabbed) Grabbed->GetState()->Hints|=HINT_AALIAS;
    else Fluxus->GetRenderer()->GetState()->Hints|=HINT_AALIAS;
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::hint_none()
{
    Primitive *Grabbed=Fluxus->GetRenderer()->Grabbed();
    if (Grabbed) Grabbed->GetState()->Hints=0;
    else Fluxus->GetRenderer()->GetState()->Hints=0;
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::blur(SCM s_blur)
{
    SCM_ASSERT(SCM_NUMBERP(s_blur), s_blur, SCM_ARG1, "blur amount");
	double blur;
	blur=gh_scm2double(s_blur);	
	if (!blur) Fluxus->GetRenderer()->SetMotionBlur(false);
    else Fluxus->GetRenderer()->SetMotionBlur(true, (float)blur);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::push()
{
    Fluxus->GetRenderer()->PushState();
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::pop()
{
    Fluxus->GetRenderer()->PopState();
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::collisions(SCM s)
{
	SCM_ASSERT(SCM_NUMBERP(s), s, SCM_ARG1, "set");
	Fluxus->GetPhysics()->SetCollisions(gh_scm2double(s));
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::ground_plane(SCM s_ori, SCM s_off)
{
    SCM_ASSERT(SCM_VECTORP(s_ori), s_ori, SCM_ARG1, "orientation");
    SCM_ASSERT(SCM_NUMBERP(s_off), s_off, SCM_ARG2, "offset");
    float ori[3];
	gh_scm2floats(s_ori,ori);
	float off = (float) gh_scm2double(s_off);
	Fluxus->GetPhysics()->GroundPlane(dVector(ori[0],ori[1],ori[2]),off);
	 return SCM_UNSPECIFIED;
}

SCM FluxusBinding::active_box(SCM s_name)
{
    SCM_ASSERT(SCM_NUMBERP(s_name), s_name, SCM_ARG1, "object name");
	int name=0;
	name=(int)gh_scm2double(s_name);	
	Fluxus->GetPhysics()->MakeActive(name,1.0f,Physics::BOX);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::active_cylinder(SCM s_name)
{
    SCM_ASSERT(SCM_NUMBERP(s_name), s_name, SCM_ARG1, "object name");
	int name=0;
	name=(int)gh_scm2double(s_name);	
	Fluxus->GetPhysics()->MakeActive(name,1.0f,Physics::CYLINDER);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::active_sphere(SCM s_name)
{
    SCM_ASSERT(SCM_NUMBERP(s_name), s_name, SCM_ARG1, "object name");
	int name=0;
	name=(int)gh_scm2double(s_name);	
	Fluxus->GetPhysics()->MakeActive(name,1.0f,Physics::SPHERE);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::passive_box(SCM s_name)
{
    SCM_ASSERT(SCM_NUMBERP(s_name), s_name, SCM_ARG1, "object name");
	int name=0;
	name=(int)gh_scm2double(s_name);	
	Fluxus->GetPhysics()->MakePassive(name,1.0f,Physics::BOX);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::passive_cylinder(SCM s_name)
{
    SCM_ASSERT(SCM_NUMBERP(s_name), s_name, SCM_ARG1, "object name");
	int name=0;
	name=(int)gh_scm2double(s_name);	
	Fluxus->GetPhysics()->MakePassive(name,1.0f,Physics::CYLINDER);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::passive_sphere(SCM s_name)
{
    SCM_ASSERT(SCM_NUMBERP(s_name), s_name, SCM_ARG1, "object name");
	int name=0;
	name=(int)gh_scm2double(s_name);	
	Fluxus->GetPhysics()->MakePassive(name,1.0f,Physics::SPHERE);
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::surface_params(SCM s_slip1, SCM s_slip2, SCM s_softerp, SCM s_softcfm)
{
	SCM_ASSERT(SCM_NUMBERP(s_slip1),    s_slip1,    SCM_ARG1, "slip 1");
	SCM_ASSERT(SCM_NUMBERP(s_slip2),    s_slip2,    SCM_ARG2, "slip 2");
	SCM_ASSERT(SCM_NUMBERP(s_softerp),  s_softerp,  SCM_ARG3, "softerp");
	SCM_ASSERT(SCM_NUMBERP(s_softcfm),  s_softcfm,  SCM_ARG4, "softcfm");
	Fluxus->GetPhysics()->SetGlobalSurfaceParams((float)gh_scm2double(s_slip1),(float)gh_scm2double(s_slip2),
		(float)gh_scm2double(s_softerp),(float)gh_scm2double(s_softcfm));
	return SCM_UNSPECIFIED;
}


SCM FluxusBinding::build_balljoint(SCM s_ob1, SCM s_ob2, SCM s_anchor)
{
    SCM_ASSERT(SCM_NUMBERP(s_ob1),    s_ob1,    SCM_ARG1, "object 1");
    SCM_ASSERT(SCM_NUMBERP(s_ob2),    s_ob2,    SCM_ARG2, "object 2");
    SCM_ASSERT(SCM_VECTORP(s_anchor), s_anchor, SCM_ARG3, "anchor");
    int name1=0;
	name1=(int)gh_scm2double(s_ob1);
	int name2=0;
	name2=(int)gh_scm2double(s_ob2);	
	float anchor[3];
	gh_scm2floats(s_anchor,anchor);
	return gh_double2scm(Fluxus->GetPhysics()->CreateJointBall(name1, name2, dVector(anchor[0],anchor[1],anchor[2])));
}

SCM FluxusBinding::build_hingejoint(SCM s_ob1, SCM s_ob2, SCM s_anchor, SCM s_hinge)
{
    SCM_ASSERT(SCM_NUMBERP(s_ob1),    s_ob1,    SCM_ARG1, "object 1");
    SCM_ASSERT(SCM_NUMBERP(s_ob2),    s_ob2,    SCM_ARG2, "object 2");
    SCM_ASSERT(SCM_VECTORP(s_anchor), s_anchor, SCM_ARG3, "anchor");
    SCM_ASSERT(SCM_VECTORP(s_hinge),  s_hinge,  SCM_ARG4, "hinge");

    int name1=0;
	name1=(int)gh_scm2double(s_ob1);
	int name2=0;
	name2=(int)gh_scm2double(s_ob2);
	
	float anchor[3];
	gh_scm2floats(s_anchor,anchor);
	
	dVector Hinge;
	float temp[3];
	gh_scm2floats(s_hinge,temp);
	Hinge.x=temp[0];
	Hinge.y=temp[1];
	Hinge.z=temp[2];
	
	return gh_double2scm(Fluxus->GetPhysics()->CreateJointHinge(name1, name2, dVector(anchor[0],anchor[1],anchor[2]), Hinge));
}

SCM FluxusBinding::build_sliderjoint(SCM s_ob1, SCM s_ob2, SCM s_hinge)
{
    SCM_ASSERT(SCM_NUMBERP(s_ob1),    s_ob1,    SCM_ARG1, "object 1");
    SCM_ASSERT(SCM_NUMBERP(s_ob2),    s_ob2,    SCM_ARG2, "object 2");
    SCM_ASSERT(SCM_VECTORP(s_hinge),  s_hinge,  SCM_ARG3, "hinge");

    int name1=0;
	name1=(int)gh_scm2double(s_ob1);
	int name2=0;
	name2=(int)gh_scm2double(s_ob2);
		
	dVector Hinge;
	float temp[3];
	gh_scm2floats(s_hinge,temp);
	Hinge.x=temp[0];
	Hinge.y=temp[1];
	Hinge.z=temp[2];
	
	return gh_double2scm(Fluxus->GetPhysics()->CreateJointSlider(name1, name2, Hinge));
}

SCM FluxusBinding::build_hinge2joint(SCM s_ob1, SCM s_ob2, SCM s_anchor, SCM s_hinge1, SCM s_hinge2)
{
    SCM_ASSERT(SCM_NUMBERP(s_ob1),    s_ob1,    SCM_ARG1, "object 1");
    SCM_ASSERT(SCM_NUMBERP(s_ob2),    s_ob2,    SCM_ARG2, "object 2");
    SCM_ASSERT(SCM_VECTORP(s_anchor), s_anchor, SCM_ARG3, "anchor");
    SCM_ASSERT(SCM_VECTORP(s_hinge1), s_hinge1, SCM_ARG4, "hinge 1");
    SCM_ASSERT(SCM_VECTORP(s_hinge2), s_hinge2, SCM_ARG5, "hinge 2");

    int name1=0;
	name1=(int)gh_scm2double(s_ob1);
	int name2=0;
	name2=(int)gh_scm2double(s_ob2);
	
	float anchor[3];
	gh_scm2floats(s_anchor,anchor);
	
	dVector Hinge[2];
	float temp[3];
	gh_scm2floats(s_hinge1,temp);
	Hinge[0].x=temp[0];
	Hinge[0].y=temp[1];
	Hinge[0].z=temp[2];
	
	gh_scm2floats(s_hinge2,temp);
	Hinge[1].x=temp[0];
	Hinge[1].y=temp[1];
	Hinge[1].z=temp[2];
	
	return gh_double2scm(Fluxus->GetPhysics()->CreateJointHinge2(name1, name2, dVector(anchor[0],anchor[1],anchor[2]), Hinge));
}

SCM FluxusBinding::build_amotorjoint(SCM s_ob1, SCM s_ob2, SCM s_axis)
{
    SCM_ASSERT(SCM_NUMBERP(s_ob1),    s_ob1,    SCM_ARG1, "object 1");
    SCM_ASSERT(SCM_NUMBERP(s_ob2),    s_ob2,    SCM_ARG2, "object 2");
    SCM_ASSERT(SCM_VECTORP(s_axis),   s_axis,   SCM_ARG3, "axis");
    int name1=0;
	name1=(int)gh_scm2double(s_ob1);
	int name2=0;
	name2=(int)gh_scm2double(s_ob2);	
	float axis[3];
	gh_scm2floats(s_axis,axis);
	return gh_double2scm(Fluxus->GetPhysics()->CreateJointAMotor(name1, name2, dVector(axis[0],axis[1],axis[2])));
}

SCM FluxusBinding::joint_param(SCM s_joint, SCM s_param, SCM s_value)
{
    SCM_ASSERT(SCM_NUMBERP(s_joint), s_joint, SCM_ARG1, "joint");
    SCM_ASSERT(SCM_STRINGP(s_param), s_param, SCM_ARG1, "param");
    SCM_ASSERT(SCM_NUMBERP(s_value), s_value, SCM_ARG2, "value");
    int joint=0;
	joint=(int)gh_scm2double(s_joint);
	char *param=0;
	size_t size=0;
	param=gh_scm2newstr(s_param,&size);	
    double v = gh_scm2double(s_value);
	Fluxus->GetPhysics()->SetJointParam(joint,param,v);
	free(param);
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::joint_angle(SCM s_joint, SCM s_vel, SCM s_angle)
{
    SCM_ASSERT(SCM_NUMBERP(s_joint), s_joint, SCM_ARG1, "joint");
    SCM_ASSERT(SCM_NUMBERP(s_vel),   s_vel,   SCM_ARG1, "vel");
    SCM_ASSERT(SCM_NUMBERP(s_angle), s_angle, SCM_ARG2, "angle");
	Fluxus->GetPhysics()->SetJointAngle((int)gh_scm2double(s_joint),gh_scm2double(s_vel),gh_scm2double(s_angle));
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::set_max_physical(SCM s_value)
{
    SCM_ASSERT(SCM_NUMBERP(s_value), s_value, SCM_ARG2, "value");
    double v = gh_scm2double(s_value);
    Fluxus->GetPhysics()->SetMaxObjectCount((int)v);
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::set_mass(SCM s_obj, SCM s_mass)
{
    SCM_ASSERT(SCM_NUMBERP(s_obj), s_obj, SCM_ARG1, "object");
    SCM_ASSERT(SCM_NUMBERP(s_mass), s_mass, SCM_ARG2, "mass");
    int obj=0;
	obj=(int)gh_scm2double(s_obj);
    float mass=gh_scm2double(s_mass);
	Fluxus->GetPhysics()->SetMass(obj,mass);
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::gravity(SCM s_vec)
{
	SCM_ASSERT(SCM_VECTORP(s_vec), s_vec, SCM_ARG2, "vector");
	float vec[3];
	gh_scm2floats(s_vec,vec);
	Fluxus->GetPhysics()->SetGravity(dVector(vec[0],vec[1],vec[2]));
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::kick(SCM s_obj, SCM s_vec)
{
    SCM_ASSERT(SCM_NUMBERP(s_obj), s_obj, SCM_ARG1, "object");
    SCM_ASSERT(SCM_VECTORP(s_vec), s_vec, SCM_ARG2, "vector");
    int obj=0;
	obj=(int)gh_scm2double(s_obj);
    float vec[3];
    gh_scm2floats(s_vec,vec);
	Fluxus->GetPhysics()->Kick(obj,dVector(vec[0],vec[1],vec[2]));
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::twist(SCM s_obj, SCM s_vec)
{
    SCM_ASSERT(SCM_NUMBERP(s_obj), s_obj, SCM_ARG1, "object");
    SCM_ASSERT(SCM_VECTORP(s_vec), s_vec, SCM_ARG2, "vector");
    int obj=0;
	obj=(int)gh_scm2double(s_obj);
    float vec[3];
    gh_scm2floats(s_vec,vec);
	Fluxus->GetPhysics()->Twist(obj,dVector(vec[0],vec[1],vec[2]));
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::srandom()
{
	return gh_double2scm(RandFloat());
}

SCM FluxusBinding::get_harmonic(SCM s_harm)
{
	SCM_ASSERT(SCM_NUMBERP(s_harm), s_harm, SCM_ARG1, "harmonic");	
    return gh_double2scm(Audio->GetHarmonic(gh_scm2int(s_harm)));
}

SCM FluxusBinding::load_texture(SCM s_name)
{
	SCM_ASSERT(SCM_STRINGP(s_name), s_name, SCM_ARG1, "name");
	char *name=0;
	size_t size=0;
	name=gh_scm2newstr(s_name,&size);	
	
	int id=Fluxus->GetRenderer()->LoadTexture(name);

    free(name);
    return gh_int2scm(id);
}

SCM FluxusBinding::texture(SCM s_id)
{
	SCM_ASSERT(SCM_NUMBERP(s_id), s_id, SCM_ARG1, "texture id");	
    Fluxus->GetRenderer()->GetState()->Texture=gh_scm2int(s_id);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::engine_callback(SCM s_func)
{
	SCM_ASSERT(SCM_STRINGP(s_func), s_func, SCM_ARG1, "callback");
	size_t size=0;
	char *temp=gh_scm2newstr(s_func,&size);
	CallbackString=temp;
	free(temp);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::ortho()
{
	//Fluxus->ortho();
	Fluxus->GetRenderer()->SetOrtho(true);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::persp()
{
	Fluxus->GetRenderer()->SetOrtho(false);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::frame()
{
	return gh_double2scm(FrameCount);
}

SCM FluxusBinding::reset_camera()
{
	Fluxus->ResetCamera();
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::print_scene_graph()
{
	Fluxus->GetRenderer()->PrintSceneGraph();
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::make_lifeforms(SCM s_name)
{
	SCM_ASSERT(SCM_STRINGP(s_name), s_name, SCM_ARG1, "name");
	size_t size=0;
	char *name=gh_scm2newstr(s_name,&size);SCM_ASSERT(SCM_STRINGP(s_name), s_name, SCM_ARG1, "name");
	Fluxus->AddLifeforms(name);
	free(name);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::add_lifeform(SCM s_name, SCM s_obj)
{
	SCM_ASSERT(SCM_STRINGP(s_name), s_name, SCM_ARG1, "name");
	SCM_ASSERT(SCM_NUMBERP(s_obj), s_obj, SCM_ARG2, "object");	
	size_t size=0;
	char *name=gh_scm2newstr(s_name,&size);
	int o = (int)gh_scm2double(s_obj);
	Lifeforms *life=Fluxus->GetLifeforms(name);
	if (life) life->AddLifeform(o,Fluxus->GetRenderer()->GetPrimitive(o)->GetState()->Transform);
	free(name);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::lifeform_avoidance(SCM s_name, SCM s_obj)
{
	SCM_ASSERT(SCM_STRINGP(s_name), s_name, SCM_ARG1, "name");
	SCM_ASSERT(SCM_NUMBERP(s_obj), s_obj, SCM_ARG2, "value");	
	size_t size=0;
	char *name=gh_scm2newstr(s_name,&size);
	Fluxus->GetLifeforms(name)->SetAvoidance(gh_scm2double(s_obj));
	free(name);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::lifeform_flockcentering(SCM s_name, SCM s_obj)
{
	SCM_ASSERT(SCM_STRINGP(s_name), s_name, SCM_ARG1, "name");
	SCM_ASSERT(SCM_NUMBERP(s_obj), s_obj, SCM_ARG2, "value");	
	size_t size=0;
	char *name=gh_scm2newstr(s_name,&size);
	Fluxus->GetLifeforms(name)->SetFlockCentering(gh_scm2double(s_obj));
	free(name);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::lifeform_scenecentering(SCM s_name, SCM s_obj)
{
	SCM_ASSERT(SCM_STRINGP(s_name), s_name, SCM_ARG1, "name");
	SCM_ASSERT(SCM_NUMBERP(s_obj), s_obj, SCM_ARG2, "value");	
	size_t size=0;
	char *name=gh_scm2newstr(s_name,&size);
	Fluxus->GetLifeforms(name)->SetSceneCentering(gh_scm2double(s_obj));
	free(name);
    return SCM_UNSPECIFIED;
}	

SCM FluxusBinding::lifeform_inertia(SCM s_name, SCM s_obj)
{
	SCM_ASSERT(SCM_STRINGP(s_name), s_name, SCM_ARG1, "name");
	SCM_ASSERT(SCM_NUMBERP(s_obj), s_obj, SCM_ARG2, "value");	
	size_t size=0;
	char *name=gh_scm2newstr(s_name,&size);
	Fluxus->GetLifeforms(name)->SetInertia(gh_scm2double(s_obj));
	free(name);
    return SCM_UNSPECIFIED;
}
	
SCM FluxusBinding::lifeform_scenecentre(SCM s_name, SCM s_obj)
{
	SCM_ASSERT(SCM_STRINGP(s_name), s_name, SCM_ARG1, "name");
	SCM_ASSERT(SCM_VECTORP(s_obj), s_obj, SCM_ARG2, "value");	
	size_t size=0;
	char *name=gh_scm2newstr(s_name,&size);
	float centre[3];
	gh_scm2floats(s_obj,centre);
	Fluxus->GetLifeforms(name)->SetSceneCentre(dVector(centre[0],centre[1],centre[2]));
	free(name);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::lifeform_maxspeed(SCM s_name, SCM s_obj)
{
	SCM_ASSERT(SCM_STRINGP(s_name), s_name, SCM_ARG1, "name");
	SCM_ASSERT(SCM_NUMBERP(s_obj), s_obj, SCM_ARG2, "value");	
	size_t size=0;
	char *name=gh_scm2newstr(s_name,&size);
	Fluxus->GetLifeforms(name)->SetMaxSpeed(gh_scm2double(s_obj));
	free(name);
    return SCM_UNSPECIFIED;
}
	
SCM FluxusBinding::save_frame(SCM s_name)
{
	SCM_ASSERT(SCM_STRINGP(s_name), s_name, SCM_ARG1, "name");	
    size_t size=0;
	char *name=gh_scm2newstr(s_name,&size);
	ofstream out(name,ios::binary);
	out<<*Fluxus->GetRenderer();
	free(name);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::load(SCM s_name)
{
	SCM_ASSERT(SCM_STRINGP(s_name), s_name, SCM_ARG1, "name");	
    size_t size=0;
	char *name=gh_scm2newstr(s_name,&size);
	Fluxus->LoadScript(name);
	free(name);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::save_name(SCM s_name)
{
	SCM_ASSERT(SCM_STRINGP(s_name), s_name, SCM_ARG1, "name");	
    size_t size=0;
	char *name=gh_scm2newstr(s_name,&size);
	Fluxus->SetSaveName(name);
	free(name);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::source(SCM s_name)
{
	SCM_ASSERT(SCM_STRINGP(s_name), s_name, SCM_ARG1, "name");	
    size_t size=0;
	char *name=gh_scm2newstr(s_name,&size);
	Fluxus->LoadScript(name);
	Fluxus->RunScript();
	free(name);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::gain(SCM s_gain)
{
	SCM_ASSERT(SCM_NUMBERP(s_gain), s_gain, SCM_ARG1, "gain");	
	Audio->SetGain(gh_scm2double(s_gain));
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::backfacecull(SCM s)
{
	SCM_ASSERT(SCM_NUMBERP(s), s, SCM_ARG1, "set");
	Fluxus->GetRenderer()->SetBackFaceCull(gh_scm2double(s));
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::desiredfps(SCM s)
{
	SCM_ASSERT(SCM_NUMBERP(s), s, SCM_ARG1, "set");
	Fluxus->GetRenderer()->SetDesiredFPS(gh_scm2double(s));
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::clear_colour(SCM s_vec)
{
    SCM_ASSERT(SCM_VECTORP(s_vec), s_vec, SCM_ARG1, "colour");
	float col[3];
	gh_scm2floats(s_vec,col);
    Fluxus->GetRenderer()->SetBGColour(dColour(col[0],col[1],col[2]));
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::clear_frame(SCM s_gain)
{
	SCM_ASSERT(SCM_NUMBERP(s_gain), s_gain, SCM_ARG1, "value");	
	Fluxus->GetRenderer()->SetClearFrame(gh_scm2double(s_gain));
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::turtle_prim(SCM type)
{
	SCM_ASSERT(SCM_NUMBERP(type), type, SCM_ARG1, "type");
	turtle.Prim((int)gh_scm2double(type));
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::turtle_vert()
{
	turtle.Vert();
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::turtle_build()
{
	return gh_double2scm(turtle.Build(Fluxus->GetRenderer()));
}

SCM FluxusBinding::turtle_move(SCM dist)
{
	SCM_ASSERT(SCM_NUMBERP(dist), dist, SCM_ARG1, "dist");
	turtle.Move(gh_scm2double(dist));
	return SCM_UNSPECIFIED;
}

SCM FluxusBinding::turtle_turn(SCM s_vec)
{
    SCM_ASSERT(SCM_VECTORP(s_vec), s_vec, SCM_ARG1, "colour");
	float rot[3];
	gh_scm2floats(s_vec,rot);
	turtle.Turn(dVector(rot[0],rot[1],rot[2]));
	return SCM_UNSPECIFIED;	
}

SCM FluxusBinding::turtle_reset()
{
	turtle.Reset();
	return SCM_UNSPECIFIED;	
}

SCM FluxusBinding::start_framedump(SCM s_name)
{
	SCM_ASSERT(SCM_STRINGP(s_name), s_name, SCM_ARG1, "filename");	
    size_t size=0;
	char *name=gh_scm2newstr(s_name,&size);
	Fluxus->StartDumpFrames(name);
	free(name);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::end_framedump()
{
	Fluxus->EndDumpFrames();
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::process(SCM s_wavname)
{
	SCM_ASSERT(SCM_STRINGP(s_wavname), s_wavname, SCM_ARG1, "wav filename");	
    size_t size=0;
	char *wavname=gh_scm2newstr(s_wavname,&size);
	Audio->Process(wavname);
	free(wavname);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::start_osc(SCM s_port)
{
	SCM_ASSERT(SCM_STRINGP(s_port), s_port, SCM_ARG1, "port");	
    size_t size=0;
	char *port=gh_scm2newstr(s_port,&size);
	Fluxus->StartOSC(port);
	free(port);
    return SCM_UNSPECIFIED;
}

SCM FluxusBinding::from_osc(SCM s_token)
{
	SCM_ASSERT(SCM_STRINGP(s_token), s_token, SCM_ARG1, "name");	
    size_t size=0;
	char *name=gh_scm2newstr(s_token,&size);
	SCM t=gh_double2scm(Fluxus->FromOSC(name));
	free(name);
	return t;
}

void FluxusBinding::RegisterProcs()
{
	// primitives
	gh_new_procedure("build-cube",      build_cube,  0,0,0);
    gh_new_procedure("build-plane",     build_plane, 0,0,0);
    gh_new_procedure0_2("build-cylinder", build_cylinder);
    gh_new_procedure0_2("build-sphere", build_sphere);
	gh_new_procedure4_0("build-line",   build_line);
    gh_new_procedure("draw-cube",       draw_cube,     0,0,0);
    gh_new_procedure("draw-plane",      draw_plane,    0,0,0);
    gh_new_procedure("draw-sphere",     draw_sphere,   0,0,0);
    gh_new_procedure("draw-cylinder",   draw_cylinder, 0,0,0);
	gh_new_procedure0_1("destroy",      destroy);

	// renderstate operations
	gh_new_procedure("push",            push,        0,0,0);
	gh_new_procedure("pop",             pop,         0,0,0);
	gh_new_procedure0_1("grab",         grab);
    gh_new_procedure("ungrab",          ungrab      ,0,0,0);
    gh_new_procedure("print-scene-graph",print_scene_graph,0,0,0);
	gh_new_procedure0_1("apply",        apply);
	gh_new_procedure("identity",        flux_identity, 0,0,0);
    gh_new_procedure0_1("translate",    translate);
    gh_new_procedure0_1("scale",        scale);
    gh_new_procedure0_1("rotate",       rotate);
    gh_new_procedure0_1("colour",       colour);
    gh_new_procedure0_1("opacity",      opacity);
    gh_new_procedure0_1("specular",     specular);
    gh_new_procedure0_1("ambient",      ambient);
    gh_new_procedure0_1("emissive",     emissive);
	gh_new_procedure0_1("shinyness",    shinyness);
	gh_new_procedure0_1("texture",      texture);
    gh_new_procedure("hint-solid",      hint_solid,       0,0,0);
    gh_new_procedure("hint-wire",       hint_wire,        0,0,0);
    gh_new_procedure("hint-normal",     hint_normal,      0,0,0);
    gh_new_procedure("hint-points",     hint_points,      0,0,0);
    gh_new_procedure("hint-anti-alias", hint_anti_alias,  0,0,0);
    gh_new_procedure("hint-none",       hint_none,  0,0,0);
	gh_new_procedure0_1("line-width",   line_width);
    gh_new_procedure0_1("parent",       parent);
	
	// global state operations
	gh_new_procedure("clear",           clear,       0,0,0);
	gh_new_procedure("ortho",       	ortho,   	 0,0,0);
	gh_new_procedure("persp",       	persp,   	 0,0,0);
    gh_new_procedure("reset-camera",    reset_camera,0,0,0);
	gh_new_procedure0_1("lock-camera",  lock_camera);
	gh_new_procedure0_1("clear-colour",    clear_colour);	
	gh_new_procedure0_1("clear-frame",     clear_frame);
	gh_new_procedure0_1("blur",         blur);
    gh_new_procedure0_1("show-axis",    show_axis);
    gh_new_procedure0_1("show-fps",     show_fps);
	gh_new_procedure0_1("backfacecull",    backfacecull);
	gh_new_procedure0_1("load-texture", load_texture);

	// lights
    gh_new_procedure0_1("make-light",         make_light);
    gh_new_procedure("clear-lights",       clear_lights,       0,0,0);
	gh_new_procedure0_2("light-ambient",   light_ambient);
	gh_new_procedure0_2("light-diffuse",   light_diffuse);
	gh_new_procedure0_2("light-specular",  light_specular);
	gh_new_procedure0_2("light-position",  light_position);
	
	// interpreter + misc
	gh_new_procedure0_1("load",   load);
	gh_new_procedure0_1("save-name",   save_name);
	gh_new_procedure0_1("source", source);
	gh_new_procedure0_1("key-pressed",  key_pressed);
     gh_new_procedure("frame",       	frame,  	 0,0,0);
   gh_new_procedure0_1("engine-callback", engine_callback);
    gh_new_procedure("flxrnd",          srandom,     0,0,0);
	gh_new_procedure0_1("desiredfps",          desiredfps);
	gh_new_procedure0_1("start-framedump",     start_framedump);
	gh_new_procedure("end-framedump",          end_framedump, 0,0,0);
	
	// audio
	gh_new_procedure0_1("gain",   gain);	
	gh_new_procedure0_1("get-harmonic", get_harmonic);
	gh_new_procedure0_1("gh",           get_harmonic);
	gh_new_procedure0_1("process",   	process);
	
	// turtle
	gh_new_procedure("turtle-vert",            turtle_vert,       0,0,0);
	gh_new_procedure("turtle-build",           turtle_build,       0,0,0);
	gh_new_procedure("turtle-reset",           turtle_reset,       0,0,0);
	gh_new_procedure0_1("turtle-move",         turtle_move);
	gh_new_procedure0_1("turtle-turn",         turtle_turn);
	gh_new_procedure0_1("turtle-prim",         turtle_prim);
	
	// lifeforms	
    gh_new_procedure0_1("make-lifeforms", make_lifeforms);
    gh_new_procedure0_2("add-lifeform", add_lifeform);
    gh_new_procedure0_2("lifeform-avoidance", lifeform_avoidance);
    gh_new_procedure0_2("lifeform-flockcentering", lifeform_flockcentering);
    gh_new_procedure0_2("lifeform-scenecentering", lifeform_scenecentering);
    gh_new_procedure0_2("lifeform-inertia", lifeform_inertia);
    gh_new_procedure0_2("lifeform-scenecentre", lifeform_scenecentre);
    gh_new_procedure0_2("lifeform-maxspeed", lifeform_maxspeed);
	
	// physics
	gh_new_procedure0_1("collisions", collisions);
	gh_new_procedure0_1("gravity", gravity);
	gh_new_procedure0_1("set-max-physical", set_max_physical);
    gh_new_procedure0_1("active-box",     active_box);
    gh_new_procedure0_1("active-sphere",  active_sphere);
    gh_new_procedure0_1("active-cylinder",active_cylinder);
    gh_new_procedure0_1("passive-box",     passive_box);
    gh_new_procedure0_1("passive-sphere",  passive_sphere);
    gh_new_procedure0_1("passive-cylinder",passive_cylinder);
    gh_new_procedure0_2("ground-plane", ground_plane);
    gh_new_procedure4_0("build-hingejoint", build_hingejoint);
    gh_new_procedure3_0("build-balljoint", build_balljoint);
    gh_new_procedure3_0("build-sliderjoint", build_sliderjoint);
    gh_new_procedure5_0("build-hinge2joint", build_hinge2joint);
    gh_new_procedure3_0("build-amotorjoint", build_amotorjoint);
    gh_new_procedure4_0("surface-params", surface_params);
    gh_new_procedure3_0("joint-param",  joint_param);
    gh_new_procedure3_0("joint-angle",  joint_angle);
    gh_new_procedure0_2("set-mass",     set_mass);
    gh_new_procedure0_2("kick",         kick);
    gh_new_procedure0_2("twist",        twist);
	
	gh_new_procedure0_1("save-frame",   save_frame);
	gh_new_procedure0_1("start-osc",    start_osc);
	gh_new_procedure0_1("from-osc",     from_osc);
}
