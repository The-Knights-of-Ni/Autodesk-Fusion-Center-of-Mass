#include <Core/CoreAll.h>
#include <Fusion/FusionAll.h>
#include <CAM/CAMAll.h>

/* TODO:
 * NECESSARY FOR SHIPPING QUALITY:
 * OSX version
 * print text info
 * increase performance
 * add icon
 * option to not show preview for better performance when selecting
 * remember stuff that was already calculated
 *     eg. one object is already selected and you select a second object, it will currently recompute the cm of the first
 *
 * OPTIONAL:
 * option to constantly recompute, so you can see how the cm changes as you move stuff
 * look into a better way to add the point than creating a sketch and adding a sketch point
 */

#ifdef XI_WIN

#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hmodule, DWORD reason, LPVOID reserved)
{
    switch (reason)
    {
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

#endif // XI_WIN

#include "maths.h"

using namespace adsk::core;
using namespace adsk::fusion;
using namespace adsk::cam;

Ptr<Application> app;
Ptr<UserInterface> ui;

Ptr<Component> root_component;

Ptr<Sketches> sketches = 0;
Ptr<Sketch> sketch = 0;

Ptr<Vector3D> hadamard(Ptr<Vector3D> a, Ptr<Vector3D> b)
{
    return Vector3D::create(a->x()*b->x(), a->y()*b->y(), a->z()*b->z());
}

struct ComOnDestroyEventHandler : public CommandEventHandler
{
    void notify(const Ptr<CommandEventArgs> & eventArgs) override
    {
    }
};

struct ComExecutePreviewEventHandler : public CommandEventHandler
{    
    char cm_text[1000];
    
    void notify(const Ptr<CommandEventArgs> & eventArgs) override
    {
        if(!eventArgs) return;
        
        Ptr<Command> command = eventArgs->command();
        if(!command) return;
        
        Ptr<CommandInputs> inputs = command->commandInputs();
        if(!inputs) return;
        
        Ptr<SelectionCommandInput> selection_input = inputs->itemById("selection");
        
        // Ptr<TextBoxCommandInput> center_of_mass_textbox = inputs->itemById("center_of_mass_textbox");
        // if(!center_of_mass_textbox)
        // {
        //     center_of_mass_textbox = inputs->addTextBoxCommandInput("center_of_mass_textbox",
        //                                                            "Center of Mass", "", 1, true);
        // }
        
        char * cm_text_current = cm_text;
        
        v3f center_of_mass = {0, 0, 0};
        double total_mass = 0;
        for(int b = 0; b < selection_input->selectionCount(); b++)
        {
            Ptr<BRepBody> body = selection_input->selection(b)->entity();
            Ptr<PhysicalProperties> physical_properties = body->physicalProperties();
            double mass = physical_properties->mass();
            double volume = physical_properties->volume();
            
            v3f centroid = {0, 0, 0};
            
            #if 1
            Ptr<MeshManager> mesh_manager = body->meshManager();
            Ptr<TriangleMeshCalculator> triangle_mesh_calculator = mesh_manager->createMeshCalculator();
            triangle_mesh_calculator->setQuality(NormalQualityTriangleMesh);
            Ptr<TriangleMesh> triangle_mesh = triangle_mesh_calculator->calculate();
            
            std::vector<float> vertex_buffer = triangle_mesh->nodeCoordinatesAsFloat();
            std::vector<int32> index_buffer = triangle_mesh->nodeIndices();
            int n_verts = triangle_mesh->nodeCount();
            int n_triangles = triangle_mesh->triangleCount();
            for(int i = 0; i < 3*n_triangles; i+=3)
            {
                v3f a = {
                    vertex_buffer[3*index_buffer[i]+0],
                    vertex_buffer[3*index_buffer[i]+1],
                    vertex_buffer[3*index_buffer[i]+2]
                };
                
                v3f b = {
                    vertex_buffer[3*index_buffer[i+1]+0],
                    vertex_buffer[3*index_buffer[i+1]+1],
                    vertex_buffer[3*index_buffer[i+1]+2]
                };
                
                v3f c = {
                    vertex_buffer[3*index_buffer[i+2]+0],
                    vertex_buffer[3*index_buffer[i+2]+1],
                    vertex_buffer[3*index_buffer[i+2]+2]
                };
                
                v3f normal = cross(b-a, c-a);
                centroid += (1.0f/24.0f)*hadamard(normal, hadamard(a+b, a+b)+hadamard(b+c, b+c)+hadamard(c+a, c+a));
            }
            centroid /= (2.0f*volume);
            
            #else
            Ptr<BRepFaces> faces = body->faces();
            
            for(int f = 0; f < faces->count(); f++)
            {
                Ptr<BRepFace> face = faces->item(f);
                Ptr<SurfaceEvaluator> surface_evaluator = face->evaluator();
                
                Ptr<BoundingBox2D> parameter_bounds = surface_evaluator->parametricRange();
                
                std::vector<Ptr<Point3D>> points; //ewwwwwww
                std::vector<Ptr<Vector3D>> partials_u;
                std::vector<Ptr<Vector3D>> partials_v;
                std::vector<Ptr<Point2D>> parameters;
                
                //TODO: adjust step size based on curvature for better speed and precision
                double uv_step = 0.1
                    *(parameter_bounds->maxPoint()->y()-parameter_bounds->minPoint()->y())
                    *(parameter_bounds->maxPoint()->x()-parameter_bounds->minPoint()->x())
                    /surface_evaluator->area();
                for(double u = parameter_bounds->minPoint()->x(); u < parameter_bounds->maxPoint()->x(); u += uv_step)
                {
                    for(double v = parameter_bounds->minPoint()->y(); v < parameter_bounds->maxPoint()->y(); v += uv_step)
                    {
                        Ptr<Point2D> uv = Point2D::create(u, v);
                        if(surface_evaluator->isParameterOnFace(uv))
                        {
                            parameters.push_back(uv);
                        }
                    }
                }
                surface_evaluator->getPointsAtParameters(parameters, points);
                surface_evaluator->getFirstDerivatives(parameters, partials_u, partials_v);
                
                for(int p = 0; p < parameters.size(); p++)
                {
                    auto dA = partials_u[p]->crossProduct(partials_v[p]);
                    dA->scaleBy(uv_step*uv_step);
                    centroid->add(
                        hadamard(hadamard(points[p]->asVector(), points[p]->asVector()), //<x^2, y^2, z^2>
                                 dA)); //*n*dA
                }
            }
            centroid->scaleBy(1.0/(2.0*volume));
            #endif
            
            // int n_printed = sprintf(cm_text_current, "%s: <%f, %f, %f> cm, %f g\n",
            //                         body->name().c_str(),
            //                         centroid.x, centroid.y, centroid.z,
            //                         mass/1000);
            // if(n_printed >= 0) cm_text_current += n_printed;
            
            center_of_mass += centroid*mass;
            total_mass += mass;
        }
        if(total_mass == 0.0) return;
        center_of_mass /= total_mass;
        
        // int n_printed = sprintf(cm_text_current, "total: <%f, %f, %f> cm, %f g\n",
        //                         center_of_mass.x, center_of_mass.y, center_of_mass.z,
        //                         total_mass/1000);
        // if(n_printed >= 0) cm_text_current += n_printed;
        
        // center_of_mass_textbox->formattedText(cm_text);
        
        if (sketch) sketch->deleteMe();
        
	sketch = sketches->add(root_component->xYConstructionPlane());
	if (!sketch) return;
        
        Ptr<SketchPoints> sketch_points = sketch->sketchPoints();
        if(!sketch_points) return;
        
        Ptr<SketchPoint> sketch_point = sketch_points->add(Point3D::create(center_of_mass.x, center_of_mass.y, center_of_mass.z));
        if(!sketch_point) {ui->messageBox("failed to add");return;};
    }
};

struct ComCommandCreatedEventHandler : public CommandCreatedEventHandler
{
    ComOnDestroyEventHandler on_destroy_handler;
    ComExecutePreviewEventHandler execute_preview_handler;
    
    void notify(const Ptr<CommandCreatedEventArgs> & eventArgs) override
    {
        int error;
        
        if(!eventArgs) return;
        
        Ptr<Command> command = eventArgs->command();
        if(!command) return;
        //command->isOKButtonVisible(false);
        
        Ptr<CommandEvent> on_destroy = command->destroy();
        error = on_destroy->add(&on_destroy_handler);
        if(!error) return;
        
        Ptr<CommandEvent> on_execute = command->execute();
        error = on_execute->add(&execute_preview_handler);
        if(!error) return;
        
        Ptr<CommandEvent> execute_preview = command->executePreview();
        error = execute_preview->add(&execute_preview_handler);
        if(!error) return;
        
        Ptr<CommandInputs> inputs = command->commandInputs();
        if(!inputs) return;
        
        Ptr<SelectionCommandInput> selection_input = inputs->addSelectionInput("selection", "Selection", "The bodies and components to find the center of mass of");
        if(!selection_input) return;
        selection_input->addSelectionFilter("SolidBodies");
        selection_input->setSelectionLimits(0);
        
        Ptr<TextBoxCommandInput> center_of_mass_textbox = inputs->addTextBoxCommandInput(
            "center_of_mass_textbox", "Center of Mass", "", 10, true);
        
        Ptr<Design> design = app->activeProduct();
        if(!design) {ui->messageBox("failed to activeProduct");return;};
        
        root_component = design->rootComponent();
        if(!root_component) {ui->messageBox("failed to rootComponent");return;};
        
        sketches = root_component->sketches();
        if(!sketches) return;
        
        sketch = 0;
    }
};
ComCommandCreatedEventHandler com_command_created_event_handler;

Ptr<CommandControl> com_control;

extern "C" XI_EXPORT bool run(const char* context)
{
    int error;
    
    app = Application::get();
    if (!app) return false;
    
    ui = app->userInterface();
    if (!ui) return false;
    
    Ptr<CommandDefinitions> command_definitions = ui->commandDefinitions();
    Ptr<CommandDefinition> com_command = command_definitions->addButtonDefinition("CenterOfMassCommand", "Center of Mass", "Reports the Center of Mass Data");
    Ptr<CommandCreatedEvent> com_command_created_event = com_command->commandCreated();
    error = com_command_created_event->add(&com_command_created_event_handler);
    if(!error) return false;
    
    Ptr<ToolbarPanelList> tpl = ui->allToolbarPanels();
    Ptr<ToolbarPanel> inspect_panel = tpl->itemById("InspectPanel");
    if(!inspect_panel) return false;
    Ptr<ToolbarControls> inspect_panel_controls = inspect_panel->controls();
    com_control = inspect_panel_controls->addCommand(com_command);
    
    // char temp[5000];
    // temp[0] = 0;
    // for(int i = 0; i < tpl->count(); i++)
    // {
    //     Ptr<ToolbarPanel> tp = tpl->item(i);
    //     strcat(temp, tp->id().c_str());
    //     strcat(temp, "\n");
    // }
    // ui->messageBox(temp);
    
    return true;
}

extern "C" XI_EXPORT bool stop(const char* context)
{
    if (ui)
    {
        Ptr<CommandDefinitions> command_definitions = ui->commandDefinitions();
        Ptr<CommandDefinition> com_command = command_definitions->itemById("CenterOfMassCommand");
        if(com_command) com_command->deleteMe();
        
        if(com_control) com_control->deleteMe();
        
        //ui->messageBox("in stop");
        ui = nullptr;
    }
    
    return true;
}
