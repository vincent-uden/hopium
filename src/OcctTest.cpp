#include "OcctTest.h"

void createBottle() {
  double myHeight = 0.070; // meters
  double myWidth = 0.050; // meters
  double myThickness = 0.030; // meters

  // Temporary points for bottle base
  gp_Pnt aPnt1(-myWidth / 2.0, 0, 0);
  gp_Pnt aPnt2(-myWidth / 2.0, -myThickness / 4.0, 0);
  gp_Pnt aPnt3(0, -myThickness / 2.0, 0);
  gp_Pnt aPnt4(myWidth / 2.0, -myThickness / 4.0, 0);
  gp_Pnt aPnt5(myWidth / 2.0, 0, 0);

  // Permanent segments and arcs
  Handle(Geom_TrimmedCurve) aArcOfCircle = GC_MakeArcOfCircle(aPnt2, aPnt3, aPnt4);
  Handle(Geom_TrimmedCurve) aSegment1 = GC_MakeSegment(aPnt1, aPnt2);
  Handle(Geom_TrimmedCurve) aSegment2 = GC_MakeSegment(aPnt4, aPnt5);

  // Error handling can be performed for **all** GC classes in case a
  // construction wasn't possible.

  // We use topological information to define the relationsips between the
  // previously disconnected geometric entities.
  TopoDS_Edge anEdge1 = BRepBuilderAPI_MakeEdge(aSegment1);
  TopoDS_Edge anEdge2 = BRepBuilderAPI_MakeEdge(aArcOfCircle);
  TopoDS_Edge anEdge3 = BRepBuilderAPI_MakeEdge(aSegment2);

  TopoDS_Wire aWire = BRepBuilderAPI_MakeWire(anEdge1, anEdge2, anEdge3);

  // Define a mirroring transformation
  gp_Pnt aOrigin(0, 0, 0);
  gp_Dir xDir(1, 0, 0);
  gp_Ax1 xAxis(aOrigin, xDir);

  gp_Trsf aTrsf;
  aTrsf.SetMirror(xAxis);

  // Apply the transformation
  BRepBuilderAPI_Transform aBRepTrsf(aWire, aTrsf);
  // The transformaed topological object could be anything, therefor we use the
  // superclass TopoDS_Shape even though we know it's another Wire.
  TopoDS_Shape aMirroredShape = aBRepTrsf.Shape();
  TopoDS_Wire aMirroredWire = TopoDS::Wire(aMirroredShape);

  // Now we need to connect the original wire with the mirrored one.
  BRepBuilderAPI_MakeWire mkWire;
  mkWire.Add(aWire);
  mkWire.Add(aMirroredWire); // The downcast was necessary for this line
  TopoDS_Wire myWireProfile = mkWire.Wire();

  // Extrude the profile by making a *prism*
  // If we didn't make this into a face, we'd get a "hollow" shell instead of a
  // solid.
  TopoDS_Face myFaceProfile = BRepBuilderAPI_MakeFace(myWireProfile);

  gp_Vec aPrismVec(0, 0, myHeight);

  TopoDS_Shape myBody = BRepPrimAPI_MakePrism(myFaceProfile, aPrismVec);

  // Fillet
  BRepFilletAPI_MakeFillet mkFillet(myBody);
  TopExp_Explorer anEdgeExplorer(myBody, TopAbs_EDGE);

  while (anEdgeExplorer.More()) {
    TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExplorer.Current());
    mkFillet.Add(myThickness / 12.0, anEdge);

    anEdgeExplorer.Next();
  }

  myBody = mkFillet.Shape();

  // Triangulate the body
  IMeshTools_Parameters params;
  params.Deflection               = 0.01;
  params.Angle                    = 0.1;
  params.Relative                 = Standard_False;
  params.InParallel               = Standard_True;
  params.MinSize                  = Precision::Confusion();
  params.InternalVerticesMode     = Standard_True;
  params.ControlSurfaceDeflection = Standard_True;
  BRepMesh_IncrementalMesh myMesh(myBody, params);
  myMesh.Perform();

  StlAPI_Writer writer = StlAPI_Writer();
  Standard_Boolean result = writer.Write(myBody, "./Bottle.stl");
  std::cout << "Bottle created " << result << std::endl;
}
