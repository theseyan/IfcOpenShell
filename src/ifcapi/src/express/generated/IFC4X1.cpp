// THIS FILE IS AUTO-GENERATED. DO NOT EDIT.
// Source: IFC4X1.exp
// Schema: IFC4X1

#include "ifcapi/express/runtime.h"
#include <cmath>

namespace ifcapi { namespace express { namespace IFC4X1 {

using ifcapi::express::Value;
using ifcapi::express::EntityRef;

// --- EXPRESS literals ---
inline const Value unknown = Value();  // EXPRESS unknown logical
inline const Value indeterminate = Value();

// --- enum constants ---
inline const Value email = Value(std::string("EMAIL"));
inline const Value fax = Value(std::string("FAX"));
inline const Value phone = Value(std::string("PHONE"));
inline const Value post = Value(std::string("POST"));
inline const Value verbal = Value(std::string("VERBAL"));
inline const Value userdefined = Value(std::string("USERDEFINED"));
inline const Value notdefined = Value(std::string("NOTDEFINED"));
inline const Value dead_load_g = Value(std::string("DEAD_LOAD_G"));
inline const Value completion_g1 = Value(std::string("COMPLETION_G1"));
inline const Value live_load_q = Value(std::string("LIVE_LOAD_Q"));
inline const Value snow_s = Value(std::string("SNOW_S"));
inline const Value wind_w = Value(std::string("WIND_W"));
inline const Value prestressing_p = Value(std::string("PRESTRESSING_P"));
inline const Value settlement_u = Value(std::string("SETTLEMENT_U"));
inline const Value temperature_t = Value(std::string("TEMPERATURE_T"));
inline const Value earthquake_e = Value(std::string("EARTHQUAKE_E"));
inline const Value fire = Value(std::string("FIRE"));
inline const Value impulse = Value(std::string("IMPULSE"));
inline const Value impact = Value(std::string("IMPACT"));
inline const Value transport = Value(std::string("TRANSPORT"));
inline const Value erection = Value(std::string("ERECTION"));
inline const Value propping = Value(std::string("PROPPING"));
inline const Value system_imperfection = Value(std::string("SYSTEM_IMPERFECTION"));
inline const Value shrinkage = Value(std::string("SHRINKAGE"));
inline const Value creep = Value(std::string("CREEP"));
inline const Value lack_of_fit = Value(std::string("LACK_OF_FIT"));
inline const Value buoyancy = Value(std::string("BUOYANCY"));
inline const Value ice = Value(std::string("ICE"));
inline const Value current = Value(std::string("CURRENT"));
inline const Value wave = Value(std::string("WAVE"));
inline const Value rain = Value(std::string("RAIN"));
inline const Value brakes = Value(std::string("BRAKES"));
inline const Value permanent_g = Value(std::string("PERMANENT_G"));
inline const Value variable_q = Value(std::string("VARIABLE_Q"));
inline const Value extraordinary_a = Value(std::string("EXTRAORDINARY_A"));
inline const Value electricactuator = Value(std::string("ELECTRICACTUATOR"));
inline const Value handoperatedactuator = Value(std::string("HANDOPERATEDACTUATOR"));
inline const Value hydraulicactuator = Value(std::string("HYDRAULICACTUATOR"));
inline const Value pneumaticactuator = Value(std::string("PNEUMATICACTUATOR"));
inline const Value thermostaticactuator = Value(std::string("THERMOSTATICACTUATOR"));
inline const Value office = Value(std::string("OFFICE"));
inline const Value site = Value(std::string("SITE"));
inline const Value home = Value(std::string("HOME"));
inline const Value distributionpoint = Value(std::string("DISTRIBUTIONPOINT"));
inline const Value constantflow = Value(std::string("CONSTANTFLOW"));
inline const Value variableflowpressuredependant = Value(std::string("VARIABLEFLOWPRESSUREDEPENDANT"));
inline const Value variableflowpressureindependant = Value(std::string("VARIABLEFLOWPRESSUREINDEPENDANT"));
inline const Value diffuser = Value(std::string("DIFFUSER"));
inline const Value grille = Value(std::string("GRILLE"));
inline const Value louvre = Value(std::string("LOUVRE"));
inline const Value register_ = Value(std::string("REGISTER"));
inline const Value fixedplatecounterflowexchanger = Value(std::string("FIXEDPLATECOUNTERFLOWEXCHANGER"));
inline const Value fixedplatecrossflowexchanger = Value(std::string("FIXEDPLATECROSSFLOWEXCHANGER"));
inline const Value fixedplateparallelflowexchanger = Value(std::string("FIXEDPLATEPARALLELFLOWEXCHANGER"));
inline const Value rotarywheel = Value(std::string("ROTARYWHEEL"));
inline const Value runaroundcoilloop = Value(std::string("RUNAROUNDCOILLOOP"));
inline const Value heatpipe = Value(std::string("HEATPIPE"));
inline const Value twintowerenthalpyrecoveryloops = Value(std::string("TWINTOWERENTHALPYRECOVERYLOOPS"));
inline const Value thermosiphonsealedtubeheatexchangers = Value(std::string("THERMOSIPHONSEALEDTUBEHEATEXCHANGERS"));
inline const Value thermosiphoncoiltypeheatexchangers = Value(std::string("THERMOSIPHONCOILTYPEHEATEXCHANGERS"));
inline const Value bell = Value(std::string("BELL"));
inline const Value breakglassbutton = Value(std::string("BREAKGLASSBUTTON"));
inline const Value light = Value(std::string("LIGHT"));
inline const Value manualpullbox = Value(std::string("MANUALPULLBOX"));
inline const Value siren = Value(std::string("SIREN"));
inline const Value whistle = Value(std::string("WHISTLE"));
inline const Value in_plane_loading_2d = Value(std::string("IN_PLANE_LOADING_2D"));
inline const Value out_plane_loading_2d = Value(std::string("OUT_PLANE_LOADING_2D"));
inline const Value loading_3d = Value(std::string("LOADING_3D"));
inline const Value first_order_theory = Value(std::string("FIRST_ORDER_THEORY"));
inline const Value second_order_theory = Value(std::string("SECOND_ORDER_THEORY"));
inline const Value third_order_theory = Value(std::string("THIRD_ORDER_THEORY"));
inline const Value full_nonlinear_theory = Value(std::string("FULL_NONLINEAR_THEORY"));
inline const Value add = Value(std::string("ADD"));
inline const Value divide = Value(std::string("DIVIDE"));
inline const Value multiply = Value(std::string("MULTIPLY"));
inline const Value subtract = Value(std::string("SUBTRACT"));
inline const Value factory = Value(std::string("FACTORY"));
inline const Value amplifier = Value(std::string("AMPLIFIER"));
inline const Value camera = Value(std::string("CAMERA"));
inline const Value display = Value(std::string("DISPLAY"));
inline const Value microphone = Value(std::string("MICROPHONE"));
inline const Value player = Value(std::string("PLAYER"));
inline const Value projector = Value(std::string("PROJECTOR"));
inline const Value receiver = Value(std::string("RECEIVER"));
inline const Value speaker = Value(std::string("SPEAKER"));
inline const Value switcher = Value(std::string("SWITCHER"));
inline const Value telephone = Value(std::string("TELEPHONE"));
inline const Value tuner = Value(std::string("TUNER"));
inline const Value polyline_form = Value(std::string("POLYLINE_FORM"));
inline const Value circular_arc = Value(std::string("CIRCULAR_ARC"));
inline const Value elliptic_arc = Value(std::string("ELLIPTIC_ARC"));
inline const Value parabolic_arc = Value(std::string("PARABOLIC_ARC"));
inline const Value hyperbolic_arc = Value(std::string("HYPERBOLIC_ARC"));
inline const Value unspecified = Value(std::string("UNSPECIFIED"));
inline const Value plane_surf = Value(std::string("PLANE_SURF"));
inline const Value cylindrical_surf = Value(std::string("CYLINDRICAL_SURF"));
inline const Value conical_surf = Value(std::string("CONICAL_SURF"));
inline const Value spherical_surf = Value(std::string("SPHERICAL_SURF"));
inline const Value toroidal_surf = Value(std::string("TOROIDAL_SURF"));
inline const Value surf_of_revolution = Value(std::string("SURF_OF_REVOLUTION"));
inline const Value ruled_surf = Value(std::string("RULED_SURF"));
inline const Value generalised_cone = Value(std::string("GENERALISED_CONE"));
inline const Value quadric_surf = Value(std::string("QUADRIC_SURF"));
inline const Value surf_of_linear_extrusion = Value(std::string("SURF_OF_LINEAR_EXTRUSION"));
inline const Value beam = Value(std::string("BEAM"));
inline const Value joist = Value(std::string("JOIST"));
inline const Value hollowcore = Value(std::string("HOLLOWCORE"));
inline const Value lintel = Value(std::string("LINTEL"));
inline const Value spandrel = Value(std::string("SPANDREL"));
inline const Value t_beam = Value(std::string("T_BEAM"));
inline const Value greaterthan = Value(std::string("GREATERTHAN"));
inline const Value greaterthanorequalto = Value(std::string("GREATERTHANOREQUALTO"));
inline const Value lessthan = Value(std::string("LESSTHAN"));
inline const Value lessthanorequalto = Value(std::string("LESSTHANOREQUALTO"));
inline const Value equalto = Value(std::string("EQUALTO"));
inline const Value notequalto = Value(std::string("NOTEQUALTO"));
inline const Value includes = Value(std::string("INCLUDES"));
inline const Value notincludes = Value(std::string("NOTINCLUDES"));
inline const Value includedin = Value(std::string("INCLUDEDIN"));
inline const Value notincludedin = Value(std::string("NOTINCLUDEDIN"));
inline const Value water = Value(std::string("WATER"));
inline const Value steam = Value(std::string("STEAM"));
inline const Value union_ = Value(std::string("UNION"));
inline const Value intersection = Value(std::string("INTERSECTION"));
inline const Value difference = Value(std::string("DIFFERENCE"));
inline const Value insulation = Value(std::string("INSULATION"));
inline const Value precastpanel = Value(std::string("PRECASTPANEL"));
inline const Value complex = Value(std::string("COMPLEX"));
inline const Value element = Value(std::string("ELEMENT"));
inline const Value partial = Value(std::string("PARTIAL"));
inline const Value provisionforvoid = Value(std::string("PROVISIONFORVOID"));
inline const Value provisionforspace = Value(std::string("PROVISIONFORSPACE"));
inline const Value fenestration = Value(std::string("FENESTRATION"));
inline const Value foundation = Value(std::string("FOUNDATION"));
inline const Value loadbearing = Value(std::string("LOADBEARING"));
inline const Value outershell = Value(std::string("OUTERSHELL"));
inline const Value shading = Value(std::string("SHADING"));
inline const Value bend = Value(std::string("BEND"));
inline const Value cross = Value(std::string("CROSS"));
inline const Value reducer = Value(std::string("REDUCER"));
inline const Value tee = Value(std::string("TEE"));
inline const Value cableladdersegment = Value(std::string("CABLELADDERSEGMENT"));
inline const Value cabletraysegment = Value(std::string("CABLETRAYSEGMENT"));
inline const Value cabletrunkingsegment = Value(std::string("CABLETRUNKINGSEGMENT"));
inline const Value conduitsegment = Value(std::string("CONDUITSEGMENT"));
inline const Value connector = Value(std::string("CONNECTOR"));
inline const Value entry = Value(std::string("ENTRY"));
inline const Value exit = Value(std::string("EXIT"));
inline const Value junction = Value(std::string("JUNCTION"));
inline const Value transition = Value(std::string("TRANSITION"));
inline const Value busbarsegment = Value(std::string("BUSBARSEGMENT"));
inline const Value cablesegment = Value(std::string("CABLESEGMENT"));
inline const Value conductorsegment = Value(std::string("CONDUCTORSEGMENT"));
inline const Value coresegment = Value(std::string("CORESEGMENT"));
inline const Value nochange = Value(std::string("NOCHANGE"));
inline const Value modified = Value(std::string("MODIFIED"));
inline const Value added = Value(std::string("ADDED"));
inline const Value deleted = Value(std::string("DELETED"));
inline const Value aircooled = Value(std::string("AIRCOOLED"));
inline const Value watercooled = Value(std::string("WATERCOOLED"));
inline const Value heatrecovery = Value(std::string("HEATRECOVERY"));
inline const Value dxcoolingcoil = Value(std::string("DXCOOLINGCOIL"));
inline const Value electricheatingcoil = Value(std::string("ELECTRICHEATINGCOIL"));
inline const Value gasheatingcoil = Value(std::string("GASHEATINGCOIL"));
inline const Value hydroniccoil = Value(std::string("HYDRONICCOIL"));
inline const Value steamheatingcoil = Value(std::string("STEAMHEATINGCOIL"));
inline const Value watercoolingcoil = Value(std::string("WATERCOOLINGCOIL"));
inline const Value waterheatingcoil = Value(std::string("WATERHEATINGCOIL"));
inline const Value column = Value(std::string("COLUMN"));
inline const Value pilaster = Value(std::string("PILASTER"));
inline const Value antenna = Value(std::string("ANTENNA"));
inline const Value computer = Value(std::string("COMPUTER"));
inline const Value gateway = Value(std::string("GATEWAY"));
inline const Value modem = Value(std::string("MODEM"));
inline const Value networkappliance = Value(std::string("NETWORKAPPLIANCE"));
inline const Value networkbridge = Value(std::string("NETWORKBRIDGE"));
inline const Value networkhub = Value(std::string("NETWORKHUB"));
inline const Value printer = Value(std::string("PRINTER"));
inline const Value repeater = Value(std::string("REPEATER"));
inline const Value router = Value(std::string("ROUTER"));
inline const Value scanner = Value(std::string("SCANNER"));
inline const Value p_complex = Value(std::string("P_COMPLEX"));
inline const Value q_complex = Value(std::string("Q_COMPLEX"));
inline const Value dynamic = Value(std::string("DYNAMIC"));
inline const Value reciprocating = Value(std::string("RECIPROCATING"));
inline const Value rotary = Value(std::string("ROTARY"));
inline const Value scroll = Value(std::string("SCROLL"));
inline const Value trochoidal = Value(std::string("TROCHOIDAL"));
inline const Value singlestage = Value(std::string("SINGLESTAGE"));
inline const Value booster = Value(std::string("BOOSTER"));
inline const Value opentype = Value(std::string("OPENTYPE"));
inline const Value hermetic = Value(std::string("HERMETIC"));
inline const Value semihermetic = Value(std::string("SEMIHERMETIC"));
inline const Value weldedshellhermetic = Value(std::string("WELDEDSHELLHERMETIC"));
inline const Value rollingpiston = Value(std::string("ROLLINGPISTON"));
inline const Value rotaryvane = Value(std::string("ROTARYVANE"));
inline const Value singlescrew = Value(std::string("SINGLESCREW"));
inline const Value twinscrew = Value(std::string("TWINSCREW"));
inline const Value evaporativecooled = Value(std::string("EVAPORATIVECOOLED"));
inline const Value watercooledbrazedplate = Value(std::string("WATERCOOLEDBRAZEDPLATE"));
inline const Value watercooledshellcoil = Value(std::string("WATERCOOLEDSHELLCOIL"));
inline const Value watercooledshelltube = Value(std::string("WATERCOOLEDSHELLTUBE"));
inline const Value watercooledtubeintube = Value(std::string("WATERCOOLEDTUBEINTUBE"));
inline const Value atpath = Value(std::string("ATPATH"));
inline const Value atstart = Value(std::string("ATSTART"));
inline const Value atend = Value(std::string("ATEND"));
inline const Value hard = Value(std::string("HARD"));
inline const Value soft = Value(std::string("SOFT"));
inline const Value advisory = Value(std::string("ADVISORY"));
inline const Value demolishing = Value(std::string("DEMOLISHING"));
inline const Value earthmoving = Value(std::string("EARTHMOVING"));
inline const Value erecting = Value(std::string("ERECTING"));
inline const Value heating = Value(std::string("HEATING"));
inline const Value lighting = Value(std::string("LIGHTING"));
inline const Value paving = Value(std::string("PAVING"));
inline const Value pumping = Value(std::string("PUMPING"));
inline const Value transporting = Value(std::string("TRANSPORTING"));
inline const Value aggregates = Value(std::string("AGGREGATES"));
inline const Value concrete = Value(std::string("CONCRETE"));
inline const Value drywall = Value(std::string("DRYWALL"));
inline const Value fuel = Value(std::string("FUEL"));
inline const Value gypsum = Value(std::string("GYPSUM"));
inline const Value masonry = Value(std::string("MASONRY"));
inline const Value metal = Value(std::string("METAL"));
inline const Value plastic = Value(std::string("PLASTIC"));
inline const Value wood = Value(std::string("WOOD"));
inline const Value assembly = Value(std::string("ASSEMBLY"));
inline const Value formwork = Value(std::string("FORMWORK"));
inline const Value floating = Value(std::string("FLOATING"));
inline const Value programmable = Value(std::string("PROGRAMMABLE"));
inline const Value proportional = Value(std::string("PROPORTIONAL"));
inline const Value multiposition = Value(std::string("MULTIPOSITION"));
inline const Value twoposition = Value(std::string("TWOPOSITION"));
inline const Value active = Value(std::string("ACTIVE"));
inline const Value passive = Value(std::string("PASSIVE"));
inline const Value naturaldraft = Value(std::string("NATURALDRAFT"));
inline const Value mechanicalinduceddraft = Value(std::string("MECHANICALINDUCEDDRAFT"));
inline const Value mechanicalforceddraft = Value(std::string("MECHANICALFORCEDDRAFT"));
inline const Value budget = Value(std::string("BUDGET"));
inline const Value costplan = Value(std::string("COSTPLAN"));
inline const Value estimate = Value(std::string("ESTIMATE"));
inline const Value tender = Value(std::string("TENDER"));
inline const Value pricedbillofquantities = Value(std::string("PRICEDBILLOFQUANTITIES"));
inline const Value unpricedbillofquantities = Value(std::string("UNPRICEDBILLOFQUANTITIES"));
inline const Value scheduleofrates = Value(std::string("SCHEDULEOFRATES"));
inline const Value ceiling = Value(std::string("CEILING"));
inline const Value flooring = Value(std::string("FLOORING"));
inline const Value cladding = Value(std::string("CLADDING"));
inline const Value roofing = Value(std::string("ROOFING"));
inline const Value molding = Value(std::string("MOLDING"));
inline const Value skirtingboard = Value(std::string("SKIRTINGBOARD"));
inline const Value membrane = Value(std::string("MEMBRANE"));
inline const Value sleeving = Value(std::string("SLEEVING"));
inline const Value wrapping = Value(std::string("WRAPPING"));
inline const Value linear = Value(std::string("LINEAR"));
inline const Value log_linear = Value(std::string("LOG_LINEAR"));
inline const Value log_log = Value(std::string("LOG_LOG"));
inline const Value backdraftdamper = Value(std::string("BACKDRAFTDAMPER"));
inline const Value balancingdamper = Value(std::string("BALANCINGDAMPER"));
inline const Value blastdamper = Value(std::string("BLASTDAMPER"));
inline const Value controldamper = Value(std::string("CONTROLDAMPER"));
inline const Value firedamper = Value(std::string("FIREDAMPER"));
inline const Value firesmokedamper = Value(std::string("FIRESMOKEDAMPER"));
inline const Value fumehoodexhaust = Value(std::string("FUMEHOODEXHAUST"));
inline const Value gravitydamper = Value(std::string("GRAVITYDAMPER"));
inline const Value gravityreliefdamper = Value(std::string("GRAVITYRELIEFDAMPER"));
inline const Value reliefdamper = Value(std::string("RELIEFDAMPER"));
inline const Value smokedamper = Value(std::string("SMOKEDAMPER"));
inline const Value measured = Value(std::string("MEASURED"));
inline const Value predicted = Value(std::string("PREDICTED"));
inline const Value simulated = Value(std::string("SIMULATED"));
inline const Value angularvelocityunit = Value(std::string("ANGULARVELOCITYUNIT"));
inline const Value areadensityunit = Value(std::string("AREADENSITYUNIT"));
inline const Value compoundplaneangleunit = Value(std::string("COMPOUNDPLANEANGLEUNIT"));
inline const Value dynamicviscosityunit = Value(std::string("DYNAMICVISCOSITYUNIT"));
inline const Value heatfluxdensityunit = Value(std::string("HEATFLUXDENSITYUNIT"));
inline const Value integercountrateunit = Value(std::string("INTEGERCOUNTRATEUNIT"));
inline const Value isothermalmoisturecapacityunit = Value(std::string("ISOTHERMALMOISTURECAPACITYUNIT"));
inline const Value kinematicviscosityunit = Value(std::string("KINEMATICVISCOSITYUNIT"));
inline const Value linearvelocityunit = Value(std::string("LINEARVELOCITYUNIT"));
inline const Value massdensityunit = Value(std::string("MASSDENSITYUNIT"));
inline const Value massflowrateunit = Value(std::string("MASSFLOWRATEUNIT"));
inline const Value moisturediffusivityunit = Value(std::string("MOISTUREDIFFUSIVITYUNIT"));
inline const Value molecularweightunit = Value(std::string("MOLECULARWEIGHTUNIT"));
inline const Value specificheatcapacityunit = Value(std::string("SPECIFICHEATCAPACITYUNIT"));
inline const Value thermaladmittanceunit = Value(std::string("THERMALADMITTANCEUNIT"));
inline const Value thermalconductanceunit = Value(std::string("THERMALCONDUCTANCEUNIT"));
inline const Value thermalresistanceunit = Value(std::string("THERMALRESISTANCEUNIT"));
inline const Value thermaltransmittanceunit = Value(std::string("THERMALTRANSMITTANCEUNIT"));
inline const Value vaporpermeabilityunit = Value(std::string("VAPORPERMEABILITYUNIT"));
inline const Value volumetricflowrateunit = Value(std::string("VOLUMETRICFLOWRATEUNIT"));
inline const Value rotationalfrequencyunit = Value(std::string("ROTATIONALFREQUENCYUNIT"));
inline const Value torqueunit = Value(std::string("TORQUEUNIT"));
inline const Value momentofinertiaunit = Value(std::string("MOMENTOFINERTIAUNIT"));
inline const Value linearmomentunit = Value(std::string("LINEARMOMENTUNIT"));
inline const Value linearforceunit = Value(std::string("LINEARFORCEUNIT"));
inline const Value planarforceunit = Value(std::string("PLANARFORCEUNIT"));
inline const Value modulusofelasticityunit = Value(std::string("MODULUSOFELASTICITYUNIT"));
inline const Value shearmodulusunit = Value(std::string("SHEARMODULUSUNIT"));
inline const Value linearstiffnessunit = Value(std::string("LINEARSTIFFNESSUNIT"));
inline const Value rotationalstiffnessunit = Value(std::string("ROTATIONALSTIFFNESSUNIT"));
inline const Value modulusofsubgradereactionunit = Value(std::string("MODULUSOFSUBGRADEREACTIONUNIT"));
inline const Value accelerationunit = Value(std::string("ACCELERATIONUNIT"));
inline const Value curvatureunit = Value(std::string("CURVATUREUNIT"));
inline const Value heatingvalueunit = Value(std::string("HEATINGVALUEUNIT"));
inline const Value ionconcentrationunit = Value(std::string("IONCONCENTRATIONUNIT"));
inline const Value luminousintensitydistributionunit = Value(std::string("LUMINOUSINTENSITYDISTRIBUTIONUNIT"));
inline const Value massperlengthunit = Value(std::string("MASSPERLENGTHUNIT"));
inline const Value modulusoflinearsubgradereactionunit = Value(std::string("MODULUSOFLINEARSUBGRADEREACTIONUNIT"));
inline const Value modulusofrotationalsubgradereactionunit = Value(std::string("MODULUSOFROTATIONALSUBGRADEREACTIONUNIT"));
inline const Value phunit = Value(std::string("PHUNIT"));
inline const Value rotationalmassunit = Value(std::string("ROTATIONALMASSUNIT"));
inline const Value sectionareaintegralunit = Value(std::string("SECTIONAREAINTEGRALUNIT"));
inline const Value sectionmodulusunit = Value(std::string("SECTIONMODULUSUNIT"));
inline const Value soundpowerlevelunit = Value(std::string("SOUNDPOWERLEVELUNIT"));
inline const Value soundpowerunit = Value(std::string("SOUNDPOWERUNIT"));
inline const Value soundpressurelevelunit = Value(std::string("SOUNDPRESSURELEVELUNIT"));
inline const Value soundpressureunit = Value(std::string("SOUNDPRESSUREUNIT"));
inline const Value temperaturegradientunit = Value(std::string("TEMPERATUREGRADIENTUNIT"));
inline const Value temperaturerateofchangeunit = Value(std::string("TEMPERATURERATEOFCHANGEUNIT"));
inline const Value thermalexpansioncoefficientunit = Value(std::string("THERMALEXPANSIONCOEFFICIENTUNIT"));
inline const Value warpingconstantunit = Value(std::string("WARPINGCONSTANTUNIT"));
inline const Value warpingmomentunit = Value(std::string("WARPINGMOMENTUNIT"));
inline const Value positive = Value(std::string("POSITIVE"));
inline const Value negative = Value(std::string("NEGATIVE"));
inline const Value anchorplate = Value(std::string("ANCHORPLATE"));
inline const Value bracket = Value(std::string("BRACKET"));
inline const Value shoe = Value(std::string("SHOE"));
inline const Value formedduct = Value(std::string("FORMEDDUCT"));
inline const Value inspectionchamber = Value(std::string("INSPECTIONCHAMBER"));
inline const Value inspectionpit = Value(std::string("INSPECTIONPIT"));
inline const Value manhole = Value(std::string("MANHOLE"));
inline const Value meterchamber = Value(std::string("METERCHAMBER"));
inline const Value sump = Value(std::string("SUMP"));
inline const Value trench = Value(std::string("TRENCH"));
inline const Value valvechamber = Value(std::string("VALVECHAMBER"));
inline const Value cable = Value(std::string("CABLE"));
inline const Value cablecarrier = Value(std::string("CABLECARRIER"));
inline const Value duct = Value(std::string("DUCT"));
inline const Value pipe = Value(std::string("PIPE"));
inline const Value airconditioning = Value(std::string("AIRCONDITIONING"));
inline const Value audiovisual = Value(std::string("AUDIOVISUAL"));
inline const Value chemical = Value(std::string("CHEMICAL"));
inline const Value chilledwater = Value(std::string("CHILLEDWATER"));
inline const Value communication = Value(std::string("COMMUNICATION"));
inline const Value compressedair = Value(std::string("COMPRESSEDAIR"));
inline const Value condenserwater = Value(std::string("CONDENSERWATER"));
inline const Value control = Value(std::string("CONTROL"));
inline const Value conveying = Value(std::string("CONVEYING"));
inline const Value data = Value(std::string("DATA"));
inline const Value disposal = Value(std::string("DISPOSAL"));
inline const Value domesticcoldwater = Value(std::string("DOMESTICCOLDWATER"));
inline const Value domestichotwater = Value(std::string("DOMESTICHOTWATER"));
inline const Value drainage = Value(std::string("DRAINAGE"));
inline const Value earthing = Value(std::string("EARTHING"));
inline const Value electrical = Value(std::string("ELECTRICAL"));
inline const Value electroacoustic = Value(std::string("ELECTROACOUSTIC"));
inline const Value exhaust = Value(std::string("EXHAUST"));
inline const Value fireprotection = Value(std::string("FIREPROTECTION"));
inline const Value gas = Value(std::string("GAS"));
inline const Value hazardous = Value(std::string("HAZARDOUS"));
inline const Value lightningprotection = Value(std::string("LIGHTNINGPROTECTION"));
inline const Value municipalsolidwaste = Value(std::string("MUNICIPALSOLIDWASTE"));
inline const Value oil = Value(std::string("OIL"));
inline const Value operational = Value(std::string("OPERATIONAL"));
inline const Value powergeneration = Value(std::string("POWERGENERATION"));
inline const Value rainwater = Value(std::string("RAINWATER"));
inline const Value refrigeration = Value(std::string("REFRIGERATION"));
inline const Value security = Value(std::string("SECURITY"));
inline const Value sewage = Value(std::string("SEWAGE"));
inline const Value signal = Value(std::string("SIGNAL"));
inline const Value stormwater = Value(std::string("STORMWATER"));
inline const Value tv = Value(std::string("TV"));
inline const Value vacuum = Value(std::string("VACUUM"));
inline const Value vent = Value(std::string("VENT"));
inline const Value ventilation = Value(std::string("VENTILATION"));
inline const Value wastewater = Value(std::string("WASTEWATER"));
inline const Value watersupply = Value(std::string("WATERSUPPLY"));
inline const Value public_ = Value(std::string("PUBLIC"));
inline const Value restricted = Value(std::string("RESTRICTED"));
inline const Value confidential = Value(std::string("CONFIDENTIAL"));
inline const Value personal = Value(std::string("PERSONAL"));
inline const Value draft = Value(std::string("DRAFT"));
inline const Value finaldraft = Value(std::string("FINALDRAFT"));
inline const Value final = Value(std::string("FINAL"));
inline const Value revision = Value(std::string("REVISION"));
inline const Value swinging = Value(std::string("SWINGING"));
inline const Value double_acting = Value(std::string("DOUBLE_ACTING"));
inline const Value sliding = Value(std::string("SLIDING"));
inline const Value folding = Value(std::string("FOLDING"));
inline const Value revolving = Value(std::string("REVOLVING"));
inline const Value rollingup = Value(std::string("ROLLINGUP"));
inline const Value fixedpanel = Value(std::string("FIXEDPANEL"));
inline const Value left = Value(std::string("LEFT"));
inline const Value middle = Value(std::string("MIDDLE"));
inline const Value right = Value(std::string("RIGHT"));
inline const Value aluminium = Value(std::string("ALUMINIUM"));
inline const Value high_grade_steel = Value(std::string("HIGH_GRADE_STEEL"));
inline const Value steel = Value(std::string("STEEL"));
inline const Value aluminium_wood = Value(std::string("ALUMINIUM_WOOD"));
inline const Value aluminium_plastic = Value(std::string("ALUMINIUM_PLASTIC"));
inline const Value single_swing_left = Value(std::string("SINGLE_SWING_LEFT"));
inline const Value single_swing_right = Value(std::string("SINGLE_SWING_RIGHT"));
inline const Value double_door_single_swing = Value(std::string("DOUBLE_DOOR_SINGLE_SWING"));
inline const Value double_door_single_swing_opposite_left = Value(std::string("DOUBLE_DOOR_SINGLE_SWING_OPPOSITE_LEFT"));
inline const Value double_door_single_swing_opposite_right = Value(std::string("DOUBLE_DOOR_SINGLE_SWING_OPPOSITE_RIGHT"));
inline const Value double_swing_left = Value(std::string("DOUBLE_SWING_LEFT"));
inline const Value double_swing_right = Value(std::string("DOUBLE_SWING_RIGHT"));
inline const Value double_door_double_swing = Value(std::string("DOUBLE_DOOR_DOUBLE_SWING"));
inline const Value sliding_to_left = Value(std::string("SLIDING_TO_LEFT"));
inline const Value sliding_to_right = Value(std::string("SLIDING_TO_RIGHT"));
inline const Value double_door_sliding = Value(std::string("DOUBLE_DOOR_SLIDING"));
inline const Value folding_to_left = Value(std::string("FOLDING_TO_LEFT"));
inline const Value folding_to_right = Value(std::string("FOLDING_TO_RIGHT"));
inline const Value double_door_folding = Value(std::string("DOUBLE_DOOR_FOLDING"));
inline const Value door = Value(std::string("DOOR"));
inline const Value gate = Value(std::string("GATE"));
inline const Value trapdoor = Value(std::string("TRAPDOOR"));
inline const Value swing_fixed_left = Value(std::string("SWING_FIXED_LEFT"));
inline const Value swing_fixed_right = Value(std::string("SWING_FIXED_RIGHT"));
inline const Value obstruction = Value(std::string("OBSTRUCTION"));
inline const Value rigidsegment = Value(std::string("RIGIDSEGMENT"));
inline const Value flexiblesegment = Value(std::string("FLEXIBLESEGMENT"));
inline const Value flatoval = Value(std::string("FLATOVAL"));
inline const Value rectangular = Value(std::string("RECTANGULAR"));
inline const Value round = Value(std::string("ROUND"));
inline const Value dishwasher = Value(std::string("DISHWASHER"));
inline const Value electriccooker = Value(std::string("ELECTRICCOOKER"));
inline const Value freestandingelectricheater = Value(std::string("FREESTANDINGELECTRICHEATER"));
inline const Value freestandingfan = Value(std::string("FREESTANDINGFAN"));
inline const Value freestandingwaterheater = Value(std::string("FREESTANDINGWATERHEATER"));
inline const Value freestandingwatercooler = Value(std::string("FREESTANDINGWATERCOOLER"));
inline const Value freezer = Value(std::string("FREEZER"));
inline const Value fridge_freezer = Value(std::string("FRIDGE_FREEZER"));
inline const Value handdryer = Value(std::string("HANDDRYER"));
inline const Value kitchenmachine = Value(std::string("KITCHENMACHINE"));
inline const Value microwave = Value(std::string("MICROWAVE"));
inline const Value photocopier = Value(std::string("PHOTOCOPIER"));
inline const Value refrigerator = Value(std::string("REFRIGERATOR"));
inline const Value tumbledryer = Value(std::string("TUMBLEDRYER"));
inline const Value vendingmachine = Value(std::string("VENDINGMACHINE"));
inline const Value washingmachine = Value(std::string("WASHINGMACHINE"));
inline const Value consumerunit = Value(std::string("CONSUMERUNIT"));
inline const Value distributionboard = Value(std::string("DISTRIBUTIONBOARD"));
inline const Value motorcontrolcentre = Value(std::string("MOTORCONTROLCENTRE"));
inline const Value switchboard = Value(std::string("SWITCHBOARD"));
inline const Value battery = Value(std::string("BATTERY"));
inline const Value capacitorbank = Value(std::string("CAPACITORBANK"));
inline const Value harmonicfilter = Value(std::string("HARMONICFILTER"));
inline const Value inductorbank = Value(std::string("INDUCTORBANK"));
inline const Value ups = Value(std::string("UPS"));
inline const Value chp = Value(std::string("CHP"));
inline const Value enginegenerator = Value(std::string("ENGINEGENERATOR"));
inline const Value standalone = Value(std::string("STANDALONE"));
inline const Value dc = Value(std::string("DC"));
inline const Value induction = Value(std::string("INDUCTION"));
inline const Value polyphase = Value(std::string("POLYPHASE"));
inline const Value reluctancesynchronous = Value(std::string("RELUCTANCESYNCHRONOUS"));
inline const Value synchronous = Value(std::string("SYNCHRONOUS"));
inline const Value timeclock = Value(std::string("TIMECLOCK"));
inline const Value timedelay = Value(std::string("TIMEDELAY"));
inline const Value relay = Value(std::string("RELAY"));
inline const Value accessory_assembly = Value(std::string("ACCESSORY_ASSEMBLY"));
inline const Value arch = Value(std::string("ARCH"));
inline const Value beam_grid = Value(std::string("BEAM_GRID"));
inline const Value braced_frame = Value(std::string("BRACED_FRAME"));
inline const Value girder = Value(std::string("GIRDER"));
inline const Value reinforcement_unit = Value(std::string("REINFORCEMENT_UNIT"));
inline const Value rigid_frame = Value(std::string("RIGID_FRAME"));
inline const Value slab_field = Value(std::string("SLAB_FIELD"));
inline const Value truss = Value(std::string("TRUSS"));
inline const Value externalcombustion = Value(std::string("EXTERNALCOMBUSTION"));
inline const Value internalcombustion = Value(std::string("INTERNALCOMBUSTION"));
inline const Value directevaporativerandommediaaircooler = Value(std::string("DIRECTEVAPORATIVERANDOMMEDIAAIRCOOLER"));
inline const Value directevaporativerigidmediaaircooler = Value(std::string("DIRECTEVAPORATIVERIGIDMEDIAAIRCOOLER"));
inline const Value directevaporativeslingerspackagedaircooler = Value(std::string("DIRECTEVAPORATIVESLINGERSPACKAGEDAIRCOOLER"));
inline const Value directevaporativepackagedrotaryaircooler = Value(std::string("DIRECTEVAPORATIVEPACKAGEDROTARYAIRCOOLER"));
inline const Value directevaporativeairwasher = Value(std::string("DIRECTEVAPORATIVEAIRWASHER"));
inline const Value indirectevaporativepackageaircooler = Value(std::string("INDIRECTEVAPORATIVEPACKAGEAIRCOOLER"));
inline const Value indirectevaporativewetcoil = Value(std::string("INDIRECTEVAPORATIVEWETCOIL"));
inline const Value indirectevaporativecoolingtowerorcoilcooler = Value(std::string("INDIRECTEVAPORATIVECOOLINGTOWERORCOILCOOLER"));
inline const Value indirectdirectcombination = Value(std::string("INDIRECTDIRECTCOMBINATION"));
inline const Value directexpansion = Value(std::string("DIRECTEXPANSION"));
inline const Value directexpansionshellandtube = Value(std::string("DIRECTEXPANSIONSHELLANDTUBE"));
inline const Value directexpansiontubeintube = Value(std::string("DIRECTEXPANSIONTUBEINTUBE"));
inline const Value directexpansionbrazedplate = Value(std::string("DIRECTEXPANSIONBRAZEDPLATE"));
inline const Value floodedshellandtube = Value(std::string("FLOODEDSHELLANDTUBE"));
inline const Value shellandcoil = Value(std::string("SHELLANDCOIL"));
inline const Value eventrule = Value(std::string("EVENTRULE"));
inline const Value eventmessage = Value(std::string("EVENTMESSAGE"));
inline const Value eventtime = Value(std::string("EVENTTIME"));
inline const Value eventcomplex = Value(std::string("EVENTCOMPLEX"));
inline const Value startevent = Value(std::string("STARTEVENT"));
inline const Value endevent = Value(std::string("ENDEVENT"));
inline const Value intermediateevent = Value(std::string("INTERMEDIATEEVENT"));
inline const Value external = Value(std::string("EXTERNAL"));
inline const Value external_earth = Value(std::string("EXTERNAL_EARTH"));
inline const Value external_water = Value(std::string("EXTERNAL_WATER"));
inline const Value external_fire = Value(std::string("EXTERNAL_FIRE"));
inline const Value centrifugalforwardcurved = Value(std::string("CENTRIFUGALFORWARDCURVED"));
inline const Value centrifugalradial = Value(std::string("CENTRIFUGALRADIAL"));
inline const Value centrifugalbackwardinclinedcurved = Value(std::string("CENTRIFUGALBACKWARDINCLINEDCURVED"));
inline const Value centrifugalairfoil = Value(std::string("CENTRIFUGALAIRFOIL"));
inline const Value tubeaxial = Value(std::string("TUBEAXIAL"));
inline const Value vaneaxial = Value(std::string("VANEAXIAL"));
inline const Value propelloraxial = Value(std::string("PROPELLORAXIAL"));
inline const Value glue = Value(std::string("GLUE"));
inline const Value mortar = Value(std::string("MORTAR"));
inline const Value weld = Value(std::string("WELD"));
inline const Value airparticlefilter = Value(std::string("AIRPARTICLEFILTER"));
inline const Value compressedairfilter = Value(std::string("COMPRESSEDAIRFILTER"));
inline const Value odorfilter = Value(std::string("ODORFILTER"));
inline const Value oilfilter = Value(std::string("OILFILTER"));
inline const Value strainer = Value(std::string("STRAINER"));
inline const Value waterfilter = Value(std::string("WATERFILTER"));
inline const Value breechinginlet = Value(std::string("BREECHINGINLET"));
inline const Value firehydrant = Value(std::string("FIREHYDRANT"));
inline const Value hosereel = Value(std::string("HOSEREEL"));
inline const Value sprinkler = Value(std::string("SPRINKLER"));
inline const Value sprinklerdeflector = Value(std::string("SPRINKLERDEFLECTOR"));
inline const Value source = Value(std::string("SOURCE"));
inline const Value sink = Value(std::string("SINK"));
inline const Value sourceandsink = Value(std::string("SOURCEANDSINK"));
inline const Value pressuregauge = Value(std::string("PRESSUREGAUGE"));
inline const Value thermometer = Value(std::string("THERMOMETER"));
inline const Value ammeter = Value(std::string("AMMETER"));
inline const Value frequencymeter = Value(std::string("FREQUENCYMETER"));
inline const Value powerfactormeter = Value(std::string("POWERFACTORMETER"));
inline const Value phaseanglemeter = Value(std::string("PHASEANGLEMETER"));
inline const Value voltmeter_peak = Value(std::string("VOLTMETER_PEAK"));
inline const Value voltmeter_rms = Value(std::string("VOLTMETER_RMS"));
inline const Value energymeter = Value(std::string("ENERGYMETER"));
inline const Value gasmeter = Value(std::string("GASMETER"));
inline const Value oilmeter = Value(std::string("OILMETER"));
inline const Value watermeter = Value(std::string("WATERMETER"));
inline const Value caisson_foundation = Value(std::string("CAISSON_FOUNDATION"));
inline const Value footing_beam = Value(std::string("FOOTING_BEAM"));
inline const Value pad_footing = Value(std::string("PAD_FOOTING"));
inline const Value pile_cap = Value(std::string("PILE_CAP"));
inline const Value strip_footing = Value(std::string("STRIP_FOOTING"));
inline const Value chair = Value(std::string("CHAIR"));
inline const Value table = Value(std::string("TABLE"));
inline const Value desk = Value(std::string("DESK"));
inline const Value bed = Value(std::string("BED"));
inline const Value filecabinet = Value(std::string("FILECABINET"));
inline const Value shelf = Value(std::string("SHELF"));
inline const Value sofa = Value(std::string("SOFA"));
inline const Value terrain = Value(std::string("TERRAIN"));
inline const Value graph_view = Value(std::string("GRAPH_VIEW"));
inline const Value sketch_view = Value(std::string("SKETCH_VIEW"));
inline const Value model_view = Value(std::string("MODEL_VIEW"));
inline const Value plan_view = Value(std::string("PLAN_VIEW"));
inline const Value reflected_plan_view = Value(std::string("REFLECTED_PLAN_VIEW"));
inline const Value section_view = Value(std::string("SECTION_VIEW"));
inline const Value elevation_view = Value(std::string("ELEVATION_VIEW"));
inline const Value global_coords = Value(std::string("GLOBAL_COORDS"));
inline const Value local_coords = Value(std::string("LOCAL_COORDS"));
inline const Value radial = Value(std::string("RADIAL"));
inline const Value triangular = Value(std::string("TRIANGULAR"));
inline const Value irregular = Value(std::string("IRREGULAR"));
inline const Value plate = Value(std::string("PLATE"));
inline const Value shellandtube = Value(std::string("SHELLANDTUBE"));
inline const Value steaminjection = Value(std::string("STEAMINJECTION"));
inline const Value adiabaticairwasher = Value(std::string("ADIABATICAIRWASHER"));
inline const Value adiabaticpan = Value(std::string("ADIABATICPAN"));
inline const Value adiabaticwettedelement = Value(std::string("ADIABATICWETTEDELEMENT"));
inline const Value adiabaticatomizing = Value(std::string("ADIABATICATOMIZING"));
inline const Value adiabaticultrasonic = Value(std::string("ADIABATICULTRASONIC"));
inline const Value adiabaticrigidmedia = Value(std::string("ADIABATICRIGIDMEDIA"));
inline const Value adiabaticcompressedairnozzle = Value(std::string("ADIABATICCOMPRESSEDAIRNOZZLE"));
inline const Value assistedelectric = Value(std::string("ASSISTEDELECTRIC"));
inline const Value assistednaturalgas = Value(std::string("ASSISTEDNATURALGAS"));
inline const Value assistedpropane = Value(std::string("ASSISTEDPROPANE"));
inline const Value assistedbutane = Value(std::string("ASSISTEDBUTANE"));
inline const Value assistedsteam = Value(std::string("ASSISTEDSTEAM"));
inline const Value cyclonic = Value(std::string("CYCLONIC"));
inline const Value grease = Value(std::string("GREASE"));
inline const Value petrol = Value(std::string("PETROL"));
inline const Value internal = Value(std::string("INTERNAL"));
inline const Value assetinventory = Value(std::string("ASSETINVENTORY"));
inline const Value spaceinventory = Value(std::string("SPACEINVENTORY"));
inline const Value furnitureinventory = Value(std::string("FURNITUREINVENTORY"));
inline const Value power = Value(std::string("POWER"));
inline const Value uniform_knots = Value(std::string("UNIFORM_KNOTS"));
inline const Value quasi_uniform_knots = Value(std::string("QUASI_UNIFORM_KNOTS"));
inline const Value piecewise_bezier_knots = Value(std::string("PIECEWISE_BEZIER_KNOTS"));
inline const Value administration = Value(std::string("ADMINISTRATION"));
inline const Value carpentry = Value(std::string("CARPENTRY"));
inline const Value cleaning = Value(std::string("CLEANING"));
inline const Value electric = Value(std::string("ELECTRIC"));
inline const Value finishing = Value(std::string("FINISHING"));
inline const Value general = Value(std::string("GENERAL"));
inline const Value hvac = Value(std::string("HVAC"));
inline const Value landscaping = Value(std::string("LANDSCAPING"));
inline const Value painting = Value(std::string("PAINTING"));
inline const Value plumbing = Value(std::string("PLUMBING"));
inline const Value sitegrading = Value(std::string("SITEGRADING"));
inline const Value steelwork = Value(std::string("STEELWORK"));
inline const Value surveying = Value(std::string("SURVEYING"));
inline const Value compactfluorescent = Value(std::string("COMPACTFLUORESCENT"));
inline const Value fluorescent = Value(std::string("FLUORESCENT"));
inline const Value halogen = Value(std::string("HALOGEN"));
inline const Value highpressuremercury = Value(std::string("HIGHPRESSUREMERCURY"));
inline const Value highpressuresodium = Value(std::string("HIGHPRESSURESODIUM"));
inline const Value led = Value(std::string("LED"));
inline const Value metalhalide = Value(std::string("METALHALIDE"));
inline const Value oled = Value(std::string("OLED"));
inline const Value tungstenfilament = Value(std::string("TUNGSTENFILAMENT"));
inline const Value axis1 = Value(std::string("AXIS1"));
inline const Value axis2 = Value(std::string("AXIS2"));
inline const Value axis3 = Value(std::string("AXIS3"));
inline const Value type_a = Value(std::string("TYPE_A"));
inline const Value type_b = Value(std::string("TYPE_B"));
inline const Value type_c = Value(std::string("TYPE_C"));
inline const Value lightemittingdiode = Value(std::string("LIGHTEMITTINGDIODE"));
inline const Value lowpressuresodium = Value(std::string("LOWPRESSURESODIUM"));
inline const Value lowvoltagehalogen = Value(std::string("LOWVOLTAGEHALOGEN"));
inline const Value mainvoltagehalogen = Value(std::string("MAINVOLTAGEHALOGEN"));
inline const Value pointsource = Value(std::string("POINTSOURCE"));
inline const Value directionsource = Value(std::string("DIRECTIONSOURCE"));
inline const Value securitylighting = Value(std::string("SECURITYLIGHTING"));
inline const Value load_group = Value(std::string("LOAD_GROUP"));
inline const Value load_case = Value(std::string("LOAD_CASE"));
inline const Value load_combination = Value(std::string("LOAD_COMBINATION"));
inline const Value logicaland = Value(std::string("LOGICALAND"));
inline const Value logicalor = Value(std::string("LOGICALOR"));
inline const Value logicalxor = Value(std::string("LOGICALXOR"));
inline const Value logicalnotand = Value(std::string("LOGICALNOTAND"));
inline const Value logicalnotor = Value(std::string("LOGICALNOTOR"));
inline const Value anchorbolt = Value(std::string("ANCHORBOLT"));
inline const Value bolt = Value(std::string("BOLT"));
inline const Value dowel = Value(std::string("DOWEL"));
inline const Value nail = Value(std::string("NAIL"));
inline const Value nailplate = Value(std::string("NAILPLATE"));
inline const Value rivet = Value(std::string("RIVET"));
inline const Value screw = Value(std::string("SCREW"));
inline const Value shearconnector = Value(std::string("SHEARCONNECTOR"));
inline const Value staple = Value(std::string("STAPLE"));
inline const Value studshearconnector = Value(std::string("STUDSHEARCONNECTOR"));
inline const Value airstation = Value(std::string("AIRSTATION"));
inline const Value feedairunit = Value(std::string("FEEDAIRUNIT"));
inline const Value oxygengenerator = Value(std::string("OXYGENGENERATOR"));
inline const Value oxygenplant = Value(std::string("OXYGENPLANT"));
inline const Value vacuumstation = Value(std::string("VACUUMSTATION"));
inline const Value brace = Value(std::string("BRACE"));
inline const Value chord = Value(std::string("CHORD"));
inline const Value collar = Value(std::string("COLLAR"));
inline const Value member = Value(std::string("MEMBER"));
inline const Value mullion = Value(std::string("MULLION"));
inline const Value purlin = Value(std::string("PURLIN"));
inline const Value rafter = Value(std::string("RAFTER"));
inline const Value stringer = Value(std::string("STRINGER"));
inline const Value strut = Value(std::string("STRUT"));
inline const Value stud = Value(std::string("STUD"));
inline const Value beltdrive = Value(std::string("BELTDRIVE"));
inline const Value coupling = Value(std::string("COUPLING"));
inline const Value directdrive = Value(std::string("DIRECTDRIVE"));
inline const Value null = Value(std::string("NULL"));
inline const Value product = Value(std::string("PRODUCT"));
inline const Value process = Value(std::string("PROCESS"));
inline const Value resource = Value(std::string("RESOURCE"));
inline const Value actor = Value(std::string("ACTOR"));
inline const Value group = Value(std::string("GROUP"));
inline const Value project = Value(std::string("PROJECT"));
inline const Value codecompliance = Value(std::string("CODECOMPLIANCE"));
inline const Value codewaiver = Value(std::string("CODEWAIVER"));
inline const Value designintent = Value(std::string("DESIGNINTENT"));
inline const Value healthandsafety = Value(std::string("HEALTHANDSAFETY"));
inline const Value mergeconflict = Value(std::string("MERGECONFLICT"));
inline const Value modelview = Value(std::string("MODELVIEW"));
inline const Value parameter = Value(std::string("PARAMETER"));
inline const Value requirement = Value(std::string("REQUIREMENT"));
inline const Value specification = Value(std::string("SPECIFICATION"));
inline const Value triggercondition = Value(std::string("TRIGGERCONDITION"));
inline const Value assignee = Value(std::string("ASSIGNEE"));
inline const Value assignor = Value(std::string("ASSIGNOR"));
inline const Value lessee = Value(std::string("LESSEE"));
inline const Value lessor = Value(std::string("LESSOR"));
inline const Value lettingagent = Value(std::string("LETTINGAGENT"));
inline const Value owner = Value(std::string("OWNER"));
inline const Value tenant = Value(std::string("TENANT"));
inline const Value opening = Value(std::string("OPENING"));
inline const Value recess = Value(std::string("RECESS"));
inline const Value audiovisualoutlet = Value(std::string("AUDIOVISUALOUTLET"));
inline const Value communicationsoutlet = Value(std::string("COMMUNICATIONSOUTLET"));
inline const Value poweroutlet = Value(std::string("POWEROUTLET"));
inline const Value dataoutlet = Value(std::string("DATAOUTLET"));
inline const Value telephoneoutlet = Value(std::string("TELEPHONEOUTLET"));
inline const Value grill = Value(std::string("GRILL"));
inline const Value louver = Value(std::string("LOUVER"));
inline const Value screen = Value(std::string("SCREEN"));
inline const Value access = Value(std::string("ACCESS"));
inline const Value building = Value(std::string("BUILDING"));
inline const Value work = Value(std::string("WORK"));
inline const Value physical = Value(std::string("PHYSICAL"));
inline const Value virtual_ = Value(std::string("VIRTUAL"));
inline const Value cast_in_place = Value(std::string("CAST_IN_PLACE"));
inline const Value composite = Value(std::string("COMPOSITE"));
inline const Value precast_concrete = Value(std::string("PRECAST_CONCRETE"));
inline const Value prefab_steel = Value(std::string("PREFAB_STEEL"));
inline const Value bored = Value(std::string("BORED"));
inline const Value driven = Value(std::string("DRIVEN"));
inline const Value jetgrouting = Value(std::string("JETGROUTING"));
inline const Value cohesion = Value(std::string("COHESION"));
inline const Value friction = Value(std::string("FRICTION"));
inline const Value support = Value(std::string("SUPPORT"));
inline const Value culvert = Value(std::string("CULVERT"));
inline const Value gutter = Value(std::string("GUTTER"));
inline const Value spool = Value(std::string("SPOOL"));
inline const Value curtain_panel = Value(std::string("CURTAIN_PANEL"));
inline const Value sheet = Value(std::string("SHEET"));
inline const Value curve3d = Value(std::string("CURVE3D"));
inline const Value pcurve_s1 = Value(std::string("PCURVE_S1"));
inline const Value pcurve_s2 = Value(std::string("PCURVE_S2"));
inline const Value advice_caution = Value(std::string("ADVICE_CAUTION"));
inline const Value advice_note = Value(std::string("ADVICE_NOTE"));
inline const Value advice_warning = Value(std::string("ADVICE_WARNING"));
inline const Value calibration = Value(std::string("CALIBRATION"));
inline const Value diagnostic = Value(std::string("DIAGNOSTIC"));
inline const Value shutdown = Value(std::string("SHUTDOWN"));
inline const Value startup = Value(std::string("STARTUP"));
inline const Value curve = Value(std::string("CURVE"));
inline const Value area = Value(std::string("AREA"));
inline const Value changeorder = Value(std::string("CHANGEORDER"));
inline const Value maintenanceworkorder = Value(std::string("MAINTENANCEWORKORDER"));
inline const Value moveorder = Value(std::string("MOVEORDER"));
inline const Value purchaseorder = Value(std::string("PURCHASEORDER"));
inline const Value workorder = Value(std::string("WORKORDER"));
inline const Value projected_length = Value(std::string("PROJECTED_LENGTH"));
inline const Value true_length = Value(std::string("TRUE_LENGTH"));
inline const Value pset_typedrivenonly = Value(std::string("PSET_TYPEDRIVENONLY"));
inline const Value pset_typedrivenoverride = Value(std::string("PSET_TYPEDRIVENOVERRIDE"));
inline const Value pset_occurrencedriven = Value(std::string("PSET_OCCURRENCEDRIVEN"));
inline const Value pset_performancedriven = Value(std::string("PSET_PERFORMANCEDRIVEN"));
inline const Value qto_typedrivenonly = Value(std::string("QTO_TYPEDRIVENONLY"));
inline const Value qto_typedrivenoverride = Value(std::string("QTO_TYPEDRIVENOVERRIDE"));
inline const Value qto_occurrencedriven = Value(std::string("QTO_OCCURRENCEDRIVEN"));
inline const Value electronic = Value(std::string("ELECTRONIC"));
inline const Value electromagnetic = Value(std::string("ELECTROMAGNETIC"));
inline const Value residualcurrent = Value(std::string("RESIDUALCURRENT"));
inline const Value thermal = Value(std::string("THERMAL"));
inline const Value circuitbreaker = Value(std::string("CIRCUITBREAKER"));
inline const Value earthleakagecircuitbreaker = Value(std::string("EARTHLEAKAGECIRCUITBREAKER"));
inline const Value earthingswitch = Value(std::string("EARTHINGSWITCH"));
inline const Value fusedisconnector = Value(std::string("FUSEDISCONNECTOR"));
inline const Value residualcurrentcircuitbreaker = Value(std::string("RESIDUALCURRENTCIRCUITBREAKER"));
inline const Value residualcurrentswitch = Value(std::string("RESIDUALCURRENTSWITCH"));
inline const Value varistor = Value(std::string("VARISTOR"));
inline const Value circulator = Value(std::string("CIRCULATOR"));
inline const Value endsuction = Value(std::string("ENDSUCTION"));
inline const Value splitcase = Value(std::string("SPLITCASE"));
inline const Value submersiblepump = Value(std::string("SUBMERSIBLEPUMP"));
inline const Value sumppump = Value(std::string("SUMPPUMP"));
inline const Value verticalinline = Value(std::string("VERTICALINLINE"));
inline const Value verticalturbine = Value(std::string("VERTICALTURBINE"));
inline const Value handrail = Value(std::string("HANDRAIL"));
inline const Value guardrail = Value(std::string("GUARDRAIL"));
inline const Value balustrade = Value(std::string("BALUSTRADE"));
inline const Value straight = Value(std::string("STRAIGHT"));
inline const Value spiral = Value(std::string("SPIRAL"));
inline const Value straight_run_ramp = Value(std::string("STRAIGHT_RUN_RAMP"));
inline const Value two_straight_run_ramp = Value(std::string("TWO_STRAIGHT_RUN_RAMP"));
inline const Value quarter_turn_ramp = Value(std::string("QUARTER_TURN_RAMP"));
inline const Value two_quarter_turn_ramp = Value(std::string("TWO_QUARTER_TURN_RAMP"));
inline const Value half_turn_ramp = Value(std::string("HALF_TURN_RAMP"));
inline const Value spiral_ramp = Value(std::string("SPIRAL_RAMP"));
inline const Value daily = Value(std::string("DAILY"));
inline const Value weekly = Value(std::string("WEEKLY"));
inline const Value monthly_by_day_of_month = Value(std::string("MONTHLY_BY_DAY_OF_MONTH"));
inline const Value monthly_by_position = Value(std::string("MONTHLY_BY_POSITION"));
inline const Value by_day_count = Value(std::string("BY_DAY_COUNT"));
inline const Value by_weekday_count = Value(std::string("BY_WEEKDAY_COUNT"));
inline const Value yearly_by_day_of_month = Value(std::string("YEARLY_BY_DAY_OF_MONTH"));
inline const Value yearly_by_position = Value(std::string("YEARLY_BY_POSITION"));
inline const Value kilopoint = Value(std::string("KILOPOINT"));
inline const Value milepoint = Value(std::string("MILEPOINT"));
inline const Value station = Value(std::string("STATION"));
inline const Value blinn = Value(std::string("BLINN"));
inline const Value flat = Value(std::string("FLAT"));
inline const Value glass = Value(std::string("GLASS"));
inline const Value matt = Value(std::string("MATT"));
inline const Value mirror = Value(std::string("MIRROR"));
inline const Value phong = Value(std::string("PHONG"));
inline const Value strauss = Value(std::string("STRAUSS"));
inline const Value main = Value(std::string("MAIN"));
inline const Value shear = Value(std::string("SHEAR"));
inline const Value ligature = Value(std::string("LIGATURE"));
inline const Value punching = Value(std::string("PUNCHING"));
inline const Value edge = Value(std::string("EDGE"));
inline const Value ring = Value(std::string("RING"));
inline const Value anchoring = Value(std::string("ANCHORING"));
inline const Value plain = Value(std::string("PLAIN"));
inline const Value textured = Value(std::string("TEXTURED"));
inline const Value supplier = Value(std::string("SUPPLIER"));
inline const Value manufacturer = Value(std::string("MANUFACTURER"));
inline const Value contractor = Value(std::string("CONTRACTOR"));
inline const Value subcontractor = Value(std::string("SUBCONTRACTOR"));
inline const Value architect = Value(std::string("ARCHITECT"));
inline const Value structuralengineer = Value(std::string("STRUCTURALENGINEER"));
inline const Value costengineer = Value(std::string("COSTENGINEER"));
inline const Value client = Value(std::string("CLIENT"));
inline const Value buildingowner = Value(std::string("BUILDINGOWNER"));
inline const Value buildingoperator = Value(std::string("BUILDINGOPERATOR"));
inline const Value mechanicalengineer = Value(std::string("MECHANICALENGINEER"));
inline const Value electricalengineer = Value(std::string("ELECTRICALENGINEER"));
inline const Value projectmanager = Value(std::string("PROJECTMANAGER"));
inline const Value facilitiesmanager = Value(std::string("FACILITIESMANAGER"));
inline const Value civilengineer = Value(std::string("CIVILENGINEER"));
inline const Value commissioningengineer = Value(std::string("COMMISSIONINGENGINEER"));
inline const Value engineer = Value(std::string("ENGINEER"));
inline const Value consultant = Value(std::string("CONSULTANT"));
inline const Value constructionmanager = Value(std::string("CONSTRUCTIONMANAGER"));
inline const Value fieldconstructionmanager = Value(std::string("FIELDCONSTRUCTIONMANAGER"));
inline const Value reseller = Value(std::string("RESELLER"));
inline const Value flat_roof = Value(std::string("FLAT_ROOF"));
inline const Value shed_roof = Value(std::string("SHED_ROOF"));
inline const Value gable_roof = Value(std::string("GABLE_ROOF"));
inline const Value hip_roof = Value(std::string("HIP_ROOF"));
inline const Value hipped_gable_roof = Value(std::string("HIPPED_GABLE_ROOF"));
inline const Value gambrel_roof = Value(std::string("GAMBREL_ROOF"));
inline const Value mansard_roof = Value(std::string("MANSARD_ROOF"));
inline const Value barrel_roof = Value(std::string("BARREL_ROOF"));
inline const Value rainbow_roof = Value(std::string("RAINBOW_ROOF"));
inline const Value butterfly_roof = Value(std::string("BUTTERFLY_ROOF"));
inline const Value pavilion_roof = Value(std::string("PAVILION_ROOF"));
inline const Value dome_roof = Value(std::string("DOME_ROOF"));
inline const Value freeform = Value(std::string("FREEFORM"));
inline const Value exa = Value(std::string("EXA"));
inline const Value peta = Value(std::string("PETA"));
inline const Value tera = Value(std::string("TERA"));
inline const Value giga = Value(std::string("GIGA"));
inline const Value mega = Value(std::string("MEGA"));
inline const Value kilo = Value(std::string("KILO"));
inline const Value hecto = Value(std::string("HECTO"));
inline const Value deca = Value(std::string("DECA"));
inline const Value deci = Value(std::string("DECI"));
inline const Value centi = Value(std::string("CENTI"));
inline const Value milli = Value(std::string("MILLI"));
inline const Value micro = Value(std::string("MICRO"));
inline const Value nano = Value(std::string("NANO"));
inline const Value pico = Value(std::string("PICO"));
inline const Value femto = Value(std::string("FEMTO"));
inline const Value atto = Value(std::string("ATTO"));
inline const Value ampere = Value(std::string("AMPERE"));
inline const Value becquerel = Value(std::string("BECQUEREL"));
inline const Value candela = Value(std::string("CANDELA"));
inline const Value coulomb = Value(std::string("COULOMB"));
inline const Value cubic_metre = Value(std::string("CUBIC_METRE"));
inline const Value degree_celsius = Value(std::string("DEGREE_CELSIUS"));
inline const Value farad = Value(std::string("FARAD"));
inline const Value gram = Value(std::string("GRAM"));
inline const Value gray = Value(std::string("GRAY"));
inline const Value henry = Value(std::string("HENRY"));
inline const Value hertz = Value(std::string("HERTZ"));
inline const Value joule = Value(std::string("JOULE"));
inline const Value kelvin = Value(std::string("KELVIN"));
inline const Value lumen = Value(std::string("LUMEN"));
inline const Value lux = Value(std::string("LUX"));
inline const Value metre = Value(std::string("METRE"));
inline const Value mole = Value(std::string("MOLE"));
inline const Value newton = Value(std::string("NEWTON"));
inline const Value ohm = Value(std::string("OHM"));
inline const Value pascal = Value(std::string("PASCAL"));
inline const Value radian = Value(std::string("RADIAN"));
inline const Value second = Value(std::string("SECOND"));
inline const Value siemens = Value(std::string("SIEMENS"));
inline const Value sievert = Value(std::string("SIEVERT"));
inline const Value square_metre = Value(std::string("SQUARE_METRE"));
inline const Value steradian = Value(std::string("STERADIAN"));
inline const Value tesla = Value(std::string("TESLA"));
inline const Value volt = Value(std::string("VOLT"));
inline const Value watt = Value(std::string("WATT"));
inline const Value weber = Value(std::string("WEBER"));
inline const Value bath = Value(std::string("BATH"));
inline const Value bidet = Value(std::string("BIDET"));
inline const Value cistern = Value(std::string("CISTERN"));
inline const Value shower = Value(std::string("SHOWER"));
inline const Value sanitaryfountain = Value(std::string("SANITARYFOUNTAIN"));
inline const Value toiletpan = Value(std::string("TOILETPAN"));
inline const Value urinal = Value(std::string("URINAL"));
inline const Value washhandbasin = Value(std::string("WASHHANDBASIN"));
inline const Value wcseat = Value(std::string("WCSEAT"));
inline const Value uniform = Value(std::string("UNIFORM"));
inline const Value tapered = Value(std::string("TAPERED"));
inline const Value cosensor = Value(std::string("COSENSOR"));
inline const Value co2sensor = Value(std::string("CO2SENSOR"));
inline const Value conductancesensor = Value(std::string("CONDUCTANCESENSOR"));
inline const Value contactsensor = Value(std::string("CONTACTSENSOR"));
inline const Value firesensor = Value(std::string("FIRESENSOR"));
inline const Value flowsensor = Value(std::string("FLOWSENSOR"));
inline const Value frostsensor = Value(std::string("FROSTSENSOR"));
inline const Value gassensor = Value(std::string("GASSENSOR"));
inline const Value heatsensor = Value(std::string("HEATSENSOR"));
inline const Value humiditysensor = Value(std::string("HUMIDITYSENSOR"));
inline const Value identifiersensor = Value(std::string("IDENTIFIERSENSOR"));
inline const Value ionconcentrationsensor = Value(std::string("IONCONCENTRATIONSENSOR"));
inline const Value levelsensor = Value(std::string("LEVELSENSOR"));
inline const Value lightsensor = Value(std::string("LIGHTSENSOR"));
inline const Value moisturesensor = Value(std::string("MOISTURESENSOR"));
inline const Value movementsensor = Value(std::string("MOVEMENTSENSOR"));
inline const Value phsensor = Value(std::string("PHSENSOR"));
inline const Value pressuresensor = Value(std::string("PRESSURESENSOR"));
inline const Value radiationsensor = Value(std::string("RADIATIONSENSOR"));
inline const Value radioactivitysensor = Value(std::string("RADIOACTIVITYSENSOR"));
inline const Value smokesensor = Value(std::string("SMOKESENSOR"));
inline const Value soundsensor = Value(std::string("SOUNDSENSOR"));
inline const Value temperaturesensor = Value(std::string("TEMPERATURESENSOR"));
inline const Value windsensor = Value(std::string("WINDSENSOR"));
inline const Value start_start = Value(std::string("START_START"));
inline const Value start_finish = Value(std::string("START_FINISH"));
inline const Value finish_start = Value(std::string("FINISH_START"));
inline const Value finish_finish = Value(std::string("FINISH_FINISH"));
inline const Value jalousie = Value(std::string("JALOUSIE"));
inline const Value shutter = Value(std::string("SHUTTER"));
inline const Value awning = Value(std::string("AWNING"));
inline const Value p_singlevalue = Value(std::string("P_SINGLEVALUE"));
inline const Value p_enumeratedvalue = Value(std::string("P_ENUMERATEDVALUE"));
inline const Value p_boundedvalue = Value(std::string("P_BOUNDEDVALUE"));
inline const Value p_listvalue = Value(std::string("P_LISTVALUE"));
inline const Value p_tablevalue = Value(std::string("P_TABLEVALUE"));
inline const Value p_referencevalue = Value(std::string("P_REFERENCEVALUE"));
inline const Value q_length = Value(std::string("Q_LENGTH"));
inline const Value q_area = Value(std::string("Q_AREA"));
inline const Value q_volume = Value(std::string("Q_VOLUME"));
inline const Value q_count = Value(std::string("Q_COUNT"));
inline const Value q_weight = Value(std::string("Q_WEIGHT"));
inline const Value q_time = Value(std::string("Q_TIME"));
inline const Value floor = Value(std::string("FLOOR"));
inline const Value roof = Value(std::string("ROOF"));
inline const Value landing = Value(std::string("LANDING"));
inline const Value baseslab = Value(std::string("BASESLAB"));
inline const Value solarcollector = Value(std::string("SOLARCOLLECTOR"));
inline const Value solarpanel = Value(std::string("SOLARPANEL"));
inline const Value convector = Value(std::string("CONVECTOR"));
inline const Value radiator = Value(std::string("RADIATOR"));
inline const Value space = Value(std::string("SPACE"));
inline const Value parking = Value(std::string("PARKING"));
inline const Value gfa = Value(std::string("GFA"));
inline const Value construction = Value(std::string("CONSTRUCTION"));
inline const Value firesafety = Value(std::string("FIRESAFETY"));
inline const Value occupancy = Value(std::string("OCCUPANCY"));
inline const Value birdcage = Value(std::string("BIRDCAGE"));
inline const Value cowl = Value(std::string("COWL"));
inline const Value rainwaterhopper = Value(std::string("RAINWATERHOPPER"));
inline const Value winder = Value(std::string("WINDER"));
inline const Value curved = Value(std::string("CURVED"));
inline const Value straight_run_stair = Value(std::string("STRAIGHT_RUN_STAIR"));
inline const Value two_straight_run_stair = Value(std::string("TWO_STRAIGHT_RUN_STAIR"));
inline const Value quarter_winding_stair = Value(std::string("QUARTER_WINDING_STAIR"));
inline const Value quarter_turn_stair = Value(std::string("QUARTER_TURN_STAIR"));
inline const Value half_winding_stair = Value(std::string("HALF_WINDING_STAIR"));
inline const Value half_turn_stair = Value(std::string("HALF_TURN_STAIR"));
inline const Value two_quarter_winding_stair = Value(std::string("TWO_QUARTER_WINDING_STAIR"));
inline const Value two_quarter_turn_stair = Value(std::string("TWO_QUARTER_TURN_STAIR"));
inline const Value three_quarter_winding_stair = Value(std::string("THREE_QUARTER_WINDING_STAIR"));
inline const Value three_quarter_turn_stair = Value(std::string("THREE_QUARTER_TURN_STAIR"));
inline const Value spiral_stair = Value(std::string("SPIRAL_STAIR"));
inline const Value double_return_stair = Value(std::string("DOUBLE_RETURN_STAIR"));
inline const Value curved_run_stair = Value(std::string("CURVED_RUN_STAIR"));
inline const Value two_curved_run_stair = Value(std::string("TWO_CURVED_RUN_STAIR"));
inline const Value readwrite = Value(std::string("READWRITE"));
inline const Value readonly = Value(std::string("READONLY"));
inline const Value locked = Value(std::string("LOCKED"));
inline const Value readwritelocked = Value(std::string("READWRITELOCKED"));
inline const Value readonlylocked = Value(std::string("READONLYLOCKED"));
inline const Value const_ = Value(std::string("CONST"));
inline const Value polygonal = Value(std::string("POLYGONAL"));
inline const Value equidistant = Value(std::string("EQUIDISTANT"));
inline const Value sinus = Value(std::string("SINUS"));
inline const Value parabola = Value(std::string("PARABOLA"));
inline const Value discrete = Value(std::string("DISCRETE"));
inline const Value rigid_joined_member = Value(std::string("RIGID_JOINED_MEMBER"));
inline const Value pin_joined_member = Value(std::string("PIN_JOINED_MEMBER"));
inline const Value tension_member = Value(std::string("TENSION_MEMBER"));
inline const Value compression_member = Value(std::string("COMPRESSION_MEMBER"));
inline const Value bilinear = Value(std::string("BILINEAR"));
inline const Value isocontour = Value(std::string("ISOCONTOUR"));
inline const Value bending_element = Value(std::string("BENDING_ELEMENT"));
inline const Value membrane_element = Value(std::string("MEMBRANE_ELEMENT"));
inline const Value shell = Value(std::string("SHELL"));
inline const Value purchase = Value(std::string("PURCHASE"));
inline const Value mark = Value(std::string("MARK"));
inline const Value tag = Value(std::string("TAG"));
inline const Value treatment = Value(std::string("TREATMENT"));
inline const Value both = Value(std::string("BOTH"));
inline const Value contactor = Value(std::string("CONTACTOR"));
inline const Value dimmerswitch = Value(std::string("DIMMERSWITCH"));
inline const Value emergencystop = Value(std::string("EMERGENCYSTOP"));
inline const Value keypad = Value(std::string("KEYPAD"));
inline const Value momentaryswitch = Value(std::string("MOMENTARYSWITCH"));
inline const Value selectorswitch = Value(std::string("SELECTORSWITCH"));
inline const Value starter = Value(std::string("STARTER"));
inline const Value switchdisconnector = Value(std::string("SWITCHDISCONNECTOR"));
inline const Value toggleswitch = Value(std::string("TOGGLESWITCH"));
inline const Value panel = Value(std::string("PANEL"));
inline const Value worksurface = Value(std::string("WORKSURFACE"));
inline const Value basin = Value(std::string("BASIN"));
inline const Value breakpressure = Value(std::string("BREAKPRESSURE"));
inline const Value expansion = Value(std::string("EXPANSION"));
inline const Value feedandexpansion = Value(std::string("FEEDANDEXPANSION"));
inline const Value pressurevessel = Value(std::string("PRESSUREVESSEL"));
inline const Value storage = Value(std::string("STORAGE"));
inline const Value vessel = Value(std::string("VESSEL"));
inline const Value elapsedtime = Value(std::string("ELAPSEDTIME"));
inline const Value worktime = Value(std::string("WORKTIME"));
inline const Value attendance = Value(std::string("ATTENDANCE"));
inline const Value demolition = Value(std::string("DEMOLITION"));
inline const Value dismantle = Value(std::string("DISMANTLE"));
inline const Value installation = Value(std::string("INSTALLATION"));
inline const Value logistic = Value(std::string("LOGISTIC"));
inline const Value maintenance = Value(std::string("MAINTENANCE"));
inline const Value move = Value(std::string("MOVE"));
inline const Value operation = Value(std::string("OPERATION"));
inline const Value removal = Value(std::string("REMOVAL"));
inline const Value renovation = Value(std::string("RENOVATION"));
inline const Value coupler = Value(std::string("COUPLER"));
inline const Value fixed_end = Value(std::string("FIXED_END"));
inline const Value tensioning_end = Value(std::string("TENSIONING_END"));
inline const Value bar = Value(std::string("BAR"));
inline const Value coated = Value(std::string("COATED"));
inline const Value strand = Value(std::string("STRAND"));
inline const Value wire = Value(std::string("WIRE"));
inline const Value up = Value(std::string("UP"));
inline const Value down = Value(std::string("DOWN"));
inline const Value continuous = Value(std::string("CONTINUOUS"));
inline const Value discretebinary = Value(std::string("DISCRETEBINARY"));
inline const Value piecewisebinary = Value(std::string("PIECEWISEBINARY"));
inline const Value piecewiseconstant = Value(std::string("PIECEWISECONSTANT"));
inline const Value piecewisecontinuous = Value(std::string("PIECEWISECONTINUOUS"));
inline const Value frequency = Value(std::string("FREQUENCY"));
inline const Value inverter = Value(std::string("INVERTER"));
inline const Value rectifier = Value(std::string("RECTIFIER"));
inline const Value voltage = Value(std::string("VOLTAGE"));
inline const Value discontinuous = Value(std::string("DISCONTINUOUS"));
inline const Value contsamegradient = Value(std::string("CONTSAMEGRADIENT"));
inline const Value contsamegradientsamecurvature = Value(std::string("CONTSAMEGRADIENTSAMECURVATURE"));
inline const Value biquadraticparabola = Value(std::string("BIQUADRATICPARABOLA"));
inline const Value blosscurve = Value(std::string("BLOSSCURVE"));
inline const Value clothoidcurve = Value(std::string("CLOTHOIDCURVE"));
inline const Value cosinecurve = Value(std::string("COSINECURVE"));
inline const Value cubicparabola = Value(std::string("CUBICPARABOLA"));
inline const Value sinecurve = Value(std::string("SINECURVE"));
inline const Value elevator = Value(std::string("ELEVATOR"));
inline const Value escalator = Value(std::string("ESCALATOR"));
inline const Value movingwalkway = Value(std::string("MOVINGWALKWAY"));
inline const Value craneway = Value(std::string("CRANEWAY"));
inline const Value liftinggear = Value(std::string("LIFTINGGEAR"));
inline const Value cartesian = Value(std::string("CARTESIAN"));
inline const Value finned = Value(std::string("FINNED"));
inline const Value absorbeddoseunit = Value(std::string("ABSORBEDDOSEUNIT"));
inline const Value amountofsubstanceunit = Value(std::string("AMOUNTOFSUBSTANCEUNIT"));
inline const Value areaunit = Value(std::string("AREAUNIT"));
inline const Value doseequivalentunit = Value(std::string("DOSEEQUIVALENTUNIT"));
inline const Value electriccapacitanceunit = Value(std::string("ELECTRICCAPACITANCEUNIT"));
inline const Value electricchargeunit = Value(std::string("ELECTRICCHARGEUNIT"));
inline const Value electricconductanceunit = Value(std::string("ELECTRICCONDUCTANCEUNIT"));
inline const Value electriccurrentunit = Value(std::string("ELECTRICCURRENTUNIT"));
inline const Value electricresistanceunit = Value(std::string("ELECTRICRESISTANCEUNIT"));
inline const Value electricvoltageunit = Value(std::string("ELECTRICVOLTAGEUNIT"));
inline const Value energyunit = Value(std::string("ENERGYUNIT"));
inline const Value forceunit = Value(std::string("FORCEUNIT"));
inline const Value frequencyunit = Value(std::string("FREQUENCYUNIT"));
inline const Value illuminanceunit = Value(std::string("ILLUMINANCEUNIT"));
inline const Value inductanceunit = Value(std::string("INDUCTANCEUNIT"));
inline const Value lengthunit = Value(std::string("LENGTHUNIT"));
inline const Value luminousfluxunit = Value(std::string("LUMINOUSFLUXUNIT"));
inline const Value luminousintensityunit = Value(std::string("LUMINOUSINTENSITYUNIT"));
inline const Value magneticfluxdensityunit = Value(std::string("MAGNETICFLUXDENSITYUNIT"));
inline const Value magneticfluxunit = Value(std::string("MAGNETICFLUXUNIT"));
inline const Value massunit = Value(std::string("MASSUNIT"));
inline const Value planeangleunit = Value(std::string("PLANEANGLEUNIT"));
inline const Value powerunit = Value(std::string("POWERUNIT"));
inline const Value pressureunit = Value(std::string("PRESSUREUNIT"));
inline const Value radioactivityunit = Value(std::string("RADIOACTIVITYUNIT"));
inline const Value solidangleunit = Value(std::string("SOLIDANGLEUNIT"));
inline const Value thermodynamictemperatureunit = Value(std::string("THERMODYNAMICTEMPERATUREUNIT"));
inline const Value timeunit = Value(std::string("TIMEUNIT"));
inline const Value volumeunit = Value(std::string("VOLUMEUNIT"));
inline const Value alarmpanel = Value(std::string("ALARMPANEL"));
inline const Value controlpanel = Value(std::string("CONTROLPANEL"));
inline const Value gasdetectionpanel = Value(std::string("GASDETECTIONPANEL"));
inline const Value indicatorpanel = Value(std::string("INDICATORPANEL"));
inline const Value mimicpanel = Value(std::string("MIMICPANEL"));
inline const Value humidistat = Value(std::string("HUMIDISTAT"));
inline const Value thermostat = Value(std::string("THERMOSTAT"));
inline const Value weatherstation = Value(std::string("WEATHERSTATION"));
inline const Value airhandler = Value(std::string("AIRHANDLER"));
inline const Value airconditioningunit = Value(std::string("AIRCONDITIONINGUNIT"));
inline const Value dehumidifier = Value(std::string("DEHUMIDIFIER"));
inline const Value splitsystem = Value(std::string("SPLITSYSTEM"));
inline const Value rooftopunit = Value(std::string("ROOFTOPUNIT"));
inline const Value airrelease = Value(std::string("AIRRELEASE"));
inline const Value antivacuum = Value(std::string("ANTIVACUUM"));
inline const Value changeover = Value(std::string("CHANGEOVER"));
inline const Value check = Value(std::string("CHECK"));
inline const Value commissioning = Value(std::string("COMMISSIONING"));
inline const Value diverting = Value(std::string("DIVERTING"));
inline const Value drawoffcock = Value(std::string("DRAWOFFCOCK"));
inline const Value doublecheck = Value(std::string("DOUBLECHECK"));
inline const Value doubleregulating = Value(std::string("DOUBLEREGULATING"));
inline const Value faucet = Value(std::string("FAUCET"));
inline const Value flushing = Value(std::string("FLUSHING"));
inline const Value gascock = Value(std::string("GASCOCK"));
inline const Value gastap = Value(std::string("GASTAP"));
inline const Value isolating = Value(std::string("ISOLATING"));
inline const Value mixing = Value(std::string("MIXING"));
inline const Value pressurereducing = Value(std::string("PRESSUREREDUCING"));
inline const Value pressurerelief = Value(std::string("PRESSURERELIEF"));
inline const Value regulating = Value(std::string("REGULATING"));
inline const Value safetycutoff = Value(std::string("SAFETYCUTOFF"));
inline const Value steamtrap = Value(std::string("STEAMTRAP"));
inline const Value stopcock = Value(std::string("STOPCOCK"));
inline const Value compression = Value(std::string("COMPRESSION"));
inline const Value spring = Value(std::string("SPRING"));
inline const Value cutout = Value(std::string("CUTOUT"));
inline const Value notch = Value(std::string("NOTCH"));
inline const Value hole = Value(std::string("HOLE"));
inline const Value miter = Value(std::string("MITER"));
inline const Value chamfer = Value(std::string("CHAMFER"));
inline const Value movable = Value(std::string("MOVABLE"));
inline const Value parapet = Value(std::string("PARAPET"));
inline const Value partitioning = Value(std::string("PARTITIONING"));
inline const Value plumbingwall = Value(std::string("PLUMBINGWALL"));
inline const Value solidwall = Value(std::string("SOLIDWALL"));
inline const Value standard = Value(std::string("STANDARD"));
inline const Value elementedwall = Value(std::string("ELEMENTEDWALL"));
inline const Value floortrap = Value(std::string("FLOORTRAP"));
inline const Value floorwaste = Value(std::string("FLOORWASTE"));
inline const Value gullysump = Value(std::string("GULLYSUMP"));
inline const Value gullytrap = Value(std::string("GULLYTRAP"));
inline const Value roofdrain = Value(std::string("ROOFDRAIN"));
inline const Value wastedisposalunit = Value(std::string("WASTEDISPOSALUNIT"));
inline const Value wastetrap = Value(std::string("WASTETRAP"));
inline const Value sidehungrighthand = Value(std::string("SIDEHUNGRIGHTHAND"));
inline const Value sidehunglefthand = Value(std::string("SIDEHUNGLEFTHAND"));
inline const Value tiltandturnrighthand = Value(std::string("TILTANDTURNRIGHTHAND"));
inline const Value tiltandturnlefthand = Value(std::string("TILTANDTURNLEFTHAND"));
inline const Value tophung = Value(std::string("TOPHUNG"));
inline const Value bottomhung = Value(std::string("BOTTOMHUNG"));
inline const Value pivothorizontal = Value(std::string("PIVOTHORIZONTAL"));
inline const Value pivotvertical = Value(std::string("PIVOTVERTICAL"));
inline const Value slidinghorizontal = Value(std::string("SLIDINGHORIZONTAL"));
inline const Value slidingvertical = Value(std::string("SLIDINGVERTICAL"));
inline const Value removablecasement = Value(std::string("REMOVABLECASEMENT"));
inline const Value fixedcasement = Value(std::string("FIXEDCASEMENT"));
inline const Value otheroperation = Value(std::string("OTHEROPERATION"));
inline const Value bottom = Value(std::string("BOTTOM"));
inline const Value top = Value(std::string("TOP"));
inline const Value other_construction = Value(std::string("OTHER_CONSTRUCTION"));
inline const Value single_panel = Value(std::string("SINGLE_PANEL"));
inline const Value double_panel_vertical = Value(std::string("DOUBLE_PANEL_VERTICAL"));
inline const Value double_panel_horizontal = Value(std::string("DOUBLE_PANEL_HORIZONTAL"));
inline const Value triple_panel_vertical = Value(std::string("TRIPLE_PANEL_VERTICAL"));
inline const Value triple_panel_bottom = Value(std::string("TRIPLE_PANEL_BOTTOM"));
inline const Value triple_panel_top = Value(std::string("TRIPLE_PANEL_TOP"));
inline const Value triple_panel_left = Value(std::string("TRIPLE_PANEL_LEFT"));
inline const Value triple_panel_right = Value(std::string("TRIPLE_PANEL_RIGHT"));
inline const Value triple_panel_horizontal = Value(std::string("TRIPLE_PANEL_HORIZONTAL"));
inline const Value window = Value(std::string("WINDOW"));
inline const Value skylight = Value(std::string("SKYLIGHT"));
inline const Value lightdome = Value(std::string("LIGHTDOME"));
inline const Value firstshift = Value(std::string("FIRSTSHIFT"));
inline const Value secondshift = Value(std::string("SECONDSHIFT"));
inline const Value thirdshift = Value(std::string("THIRDSHIFT"));
inline const Value actual = Value(std::string("ACTUAL"));
inline const Value baseline = Value(std::string("BASELINE"));
inline const Value planned = Value(std::string("PLANNED"));

// --- entity constructors ---
inline Value IfcActionRequest(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identification, Value PredefinedType, Value Status, Value LongDescription) { return ifcapi::express::make_entity("IFC4X1", "IfcActionRequest", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identification", Identification}, {"PredefinedType", PredefinedType}, {"Status", Status}, {"LongDescription", LongDescription}}); }
inline Value IfcActor(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value TheActor) { return ifcapi::express::make_entity("IFC4X1", "IfcActor", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"TheActor", TheActor}}); }
inline Value IfcActorRole(Value Role, Value UserDefinedRole, Value Description) { return ifcapi::express::make_entity("IFC4X1", "IfcActorRole", {{"Role", Role}, {"UserDefinedRole", UserDefinedRole}, {"Description", Description}}); }
inline Value IfcActuator(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcActuator", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcActuatorType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcActuatorType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcAddress(Value Purpose, Value Description, Value UserDefinedPurpose) { return ifcapi::express::make_entity("IFC4X1", "IfcAddress", {{"Purpose", Purpose}, {"Description", Description}, {"UserDefinedPurpose", UserDefinedPurpose}}); }
inline Value IfcAdvancedBrep(Value Outer) { return ifcapi::express::make_entity("IFC4X1", "IfcAdvancedBrep", {{"Outer", Outer}}); }
inline Value IfcAdvancedBrepWithVoids(Value Outer, Value Voids) { return ifcapi::express::make_entity("IFC4X1", "IfcAdvancedBrepWithVoids", {{"Outer", Outer}, {"Voids", Voids}}); }
inline Value IfcAdvancedFace(Value Bounds, Value FaceSurface, Value SameSense) { return ifcapi::express::make_entity("IFC4X1", "IfcAdvancedFace", {{"Bounds", Bounds}, {"FaceSurface", FaceSurface}, {"SameSense", SameSense}}); }
inline Value IfcAirTerminal(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcAirTerminal", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcAirTerminalBox(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcAirTerminalBox", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcAirTerminalBoxType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcAirTerminalBoxType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcAirTerminalType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcAirTerminalType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcAirToAirHeatRecovery(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcAirToAirHeatRecovery", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcAirToAirHeatRecoveryType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcAirToAirHeatRecoveryType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcAlarm(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcAlarm", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcAlarmType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcAlarmType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcAlignment(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Axis, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcAlignment", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Axis", Axis}, {"PredefinedType", PredefinedType}}); }
inline Value IfcAlignment2DHorizontal(Value StartDistAlong, Value Segments) { return ifcapi::express::make_entity("IFC4X1", "IfcAlignment2DHorizontal", {{"StartDistAlong", StartDistAlong}, {"Segments", Segments}}); }
inline Value IfcAlignment2DHorizontalSegment(Value TangentialContinuity, Value StartTag, Value EndTag, Value CurveGeometry) { return ifcapi::express::make_entity("IFC4X1", "IfcAlignment2DHorizontalSegment", {{"TangentialContinuity", TangentialContinuity}, {"StartTag", StartTag}, {"EndTag", EndTag}, {"CurveGeometry", CurveGeometry}}); }
inline Value IfcAlignment2DSegment(Value TangentialContinuity, Value StartTag, Value EndTag) { return ifcapi::express::make_entity("IFC4X1", "IfcAlignment2DSegment", {{"TangentialContinuity", TangentialContinuity}, {"StartTag", StartTag}, {"EndTag", EndTag}}); }
inline Value IfcAlignment2DVerSegCircularArc(Value TangentialContinuity, Value StartTag, Value EndTag, Value StartDistAlong, Value HorizontalLength, Value StartHeight, Value StartGradient, Value Radius, Value IsConvex) { return ifcapi::express::make_entity("IFC4X1", "IfcAlignment2DVerSegCircularArc", {{"TangentialContinuity", TangentialContinuity}, {"StartTag", StartTag}, {"EndTag", EndTag}, {"StartDistAlong", StartDistAlong}, {"HorizontalLength", HorizontalLength}, {"StartHeight", StartHeight}, {"StartGradient", StartGradient}, {"Radius", Radius}, {"IsConvex", IsConvex}}); }
inline Value IfcAlignment2DVerSegLine(Value TangentialContinuity, Value StartTag, Value EndTag, Value StartDistAlong, Value HorizontalLength, Value StartHeight, Value StartGradient) { return ifcapi::express::make_entity("IFC4X1", "IfcAlignment2DVerSegLine", {{"TangentialContinuity", TangentialContinuity}, {"StartTag", StartTag}, {"EndTag", EndTag}, {"StartDistAlong", StartDistAlong}, {"HorizontalLength", HorizontalLength}, {"StartHeight", StartHeight}, {"StartGradient", StartGradient}}); }
inline Value IfcAlignment2DVerSegParabolicArc(Value TangentialContinuity, Value StartTag, Value EndTag, Value StartDistAlong, Value HorizontalLength, Value StartHeight, Value StartGradient, Value ParabolaConstant, Value IsConvex) { return ifcapi::express::make_entity("IFC4X1", "IfcAlignment2DVerSegParabolicArc", {{"TangentialContinuity", TangentialContinuity}, {"StartTag", StartTag}, {"EndTag", EndTag}, {"StartDistAlong", StartDistAlong}, {"HorizontalLength", HorizontalLength}, {"StartHeight", StartHeight}, {"StartGradient", StartGradient}, {"ParabolaConstant", ParabolaConstant}, {"IsConvex", IsConvex}}); }
inline Value IfcAlignment2DVertical(Value Segments) { return ifcapi::express::make_entity("IFC4X1", "IfcAlignment2DVertical", {{"Segments", Segments}}); }
inline Value IfcAlignment2DVerticalSegment(Value TangentialContinuity, Value StartTag, Value EndTag, Value StartDistAlong, Value HorizontalLength, Value StartHeight, Value StartGradient) { return ifcapi::express::make_entity("IFC4X1", "IfcAlignment2DVerticalSegment", {{"TangentialContinuity", TangentialContinuity}, {"StartTag", StartTag}, {"EndTag", EndTag}, {"StartDistAlong", StartDistAlong}, {"HorizontalLength", HorizontalLength}, {"StartHeight", StartHeight}, {"StartGradient", StartGradient}}); }
inline Value IfcAlignmentCurve(Value Horizontal, Value Vertical, Value Tag) { return ifcapi::express::make_entity("IFC4X1", "IfcAlignmentCurve", {{"Horizontal", Horizontal}, {"Vertical", Vertical}, {"Tag", Tag}}); }
inline Value IfcAnnotation(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation) { return ifcapi::express::make_entity("IFC4X1", "IfcAnnotation", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}}); }
inline Value IfcAnnotationFillArea(Value OuterBoundary, Value InnerBoundaries) { return ifcapi::express::make_entity("IFC4X1", "IfcAnnotationFillArea", {{"OuterBoundary", OuterBoundary}, {"InnerBoundaries", InnerBoundaries}}); }
inline Value IfcApplication(Value ApplicationDeveloper, Value Version, Value ApplicationFullName, Value ApplicationIdentifier) { return ifcapi::express::make_entity("IFC4X1", "IfcApplication", {{"ApplicationDeveloper", ApplicationDeveloper}, {"Version", Version}, {"ApplicationFullName", ApplicationFullName}, {"ApplicationIdentifier", ApplicationIdentifier}}); }
inline Value IfcAppliedValue(Value Name, Value Description, Value AppliedValue, Value UnitBasis, Value ApplicableDate, Value FixedUntilDate, Value Category, Value Condition, Value ArithmeticOperator, Value Components) { return ifcapi::express::make_entity("IFC4X1", "IfcAppliedValue", {{"Name", Name}, {"Description", Description}, {"AppliedValue", AppliedValue}, {"UnitBasis", UnitBasis}, {"ApplicableDate", ApplicableDate}, {"FixedUntilDate", FixedUntilDate}, {"Category", Category}, {"Condition", Condition}, {"ArithmeticOperator", ArithmeticOperator}, {"Components", Components}}); }
inline Value IfcApproval(Value Identifier, Value Name, Value Description, Value TimeOfApproval, Value Status, Value Level, Value Qualifier, Value RequestingApproval, Value GivingApproval) { return ifcapi::express::make_entity("IFC4X1", "IfcApproval", {{"Identifier", Identifier}, {"Name", Name}, {"Description", Description}, {"TimeOfApproval", TimeOfApproval}, {"Status", Status}, {"Level", Level}, {"Qualifier", Qualifier}, {"RequestingApproval", RequestingApproval}, {"GivingApproval", GivingApproval}}); }
inline Value IfcApprovalRelationship(Value Name, Value Description, Value RelatingApproval, Value RelatedApprovals) { return ifcapi::express::make_entity("IFC4X1", "IfcApprovalRelationship", {{"Name", Name}, {"Description", Description}, {"RelatingApproval", RelatingApproval}, {"RelatedApprovals", RelatedApprovals}}); }
inline Value IfcArbitraryClosedProfileDef(Value ProfileType, Value ProfileName, Value OuterCurve) { return ifcapi::express::make_entity("IFC4X1", "IfcArbitraryClosedProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"OuterCurve", OuterCurve}}); }
inline Value IfcArbitraryOpenProfileDef(Value ProfileType, Value ProfileName, Value Curve) { return ifcapi::express::make_entity("IFC4X1", "IfcArbitraryOpenProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Curve", Curve}}); }
inline Value IfcArbitraryProfileDefWithVoids(Value ProfileType, Value ProfileName, Value OuterCurve, Value InnerCurves) { return ifcapi::express::make_entity("IFC4X1", "IfcArbitraryProfileDefWithVoids", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"OuterCurve", OuterCurve}, {"InnerCurves", InnerCurves}}); }
inline Value IfcAsset(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identification, Value OriginalValue, Value CurrentValue, Value TotalReplacementCost, Value Owner, Value User, Value ResponsiblePerson, Value IncorporationDate, Value DepreciatedValue) { return ifcapi::express::make_entity("IFC4X1", "IfcAsset", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identification", Identification}, {"OriginalValue", OriginalValue}, {"CurrentValue", CurrentValue}, {"TotalReplacementCost", TotalReplacementCost}, {"Owner", Owner}, {"User", User}, {"ResponsiblePerson", ResponsiblePerson}, {"IncorporationDate", IncorporationDate}, {"DepreciatedValue", DepreciatedValue}}); }
inline Value IfcAsymmetricIShapeProfileDef(Value ProfileType, Value ProfileName, Value Position, Value BottomFlangeWidth, Value OverallDepth, Value WebThickness, Value BottomFlangeThickness, Value BottomFlangeFilletRadius, Value TopFlangeWidth, Value TopFlangeThickness, Value TopFlangeFilletRadius, Value BottomFlangeEdgeRadius, Value BottomFlangeSlope, Value TopFlangeEdgeRadius, Value TopFlangeSlope) { return ifcapi::express::make_entity("IFC4X1", "IfcAsymmetricIShapeProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"BottomFlangeWidth", BottomFlangeWidth}, {"OverallDepth", OverallDepth}, {"WebThickness", WebThickness}, {"BottomFlangeThickness", BottomFlangeThickness}, {"BottomFlangeFilletRadius", BottomFlangeFilletRadius}, {"TopFlangeWidth", TopFlangeWidth}, {"TopFlangeThickness", TopFlangeThickness}, {"TopFlangeFilletRadius", TopFlangeFilletRadius}, {"BottomFlangeEdgeRadius", BottomFlangeEdgeRadius}, {"BottomFlangeSlope", BottomFlangeSlope}, {"TopFlangeEdgeRadius", TopFlangeEdgeRadius}, {"TopFlangeSlope", TopFlangeSlope}}); }
inline Value IfcAudioVisualAppliance(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcAudioVisualAppliance", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcAudioVisualApplianceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcAudioVisualApplianceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcAxis1Placement(Value Location, Value Axis) { return ifcapi::express::make_entity("IFC4X1", "IfcAxis1Placement", {{"Location", Location}, {"Axis", Axis}}); }
inline Value IfcAxis2Placement2D(Value Location, Value RefDirection) { return ifcapi::express::make_entity("IFC4X1", "IfcAxis2Placement2D", {{"Location", Location}, {"RefDirection", RefDirection}}); }
inline Value IfcAxis2Placement3D(Value Location, Value Axis, Value RefDirection) { return ifcapi::express::make_entity("IFC4X1", "IfcAxis2Placement3D", {{"Location", Location}, {"Axis", Axis}, {"RefDirection", RefDirection}}); }
inline Value IfcBSplineCurve(Value Degree, Value ControlPointsList, Value CurveForm, Value ClosedCurve, Value SelfIntersect) { return ifcapi::express::make_entity("IFC4X1", "IfcBSplineCurve", {{"Degree", Degree}, {"ControlPointsList", ControlPointsList}, {"CurveForm", CurveForm}, {"ClosedCurve", ClosedCurve}, {"SelfIntersect", SelfIntersect}}); }
inline Value IfcBSplineCurveWithKnots(Value Degree, Value ControlPointsList, Value CurveForm, Value ClosedCurve, Value SelfIntersect, Value KnotMultiplicities, Value Knots, Value KnotSpec) { return ifcapi::express::make_entity("IFC4X1", "IfcBSplineCurveWithKnots", {{"Degree", Degree}, {"ControlPointsList", ControlPointsList}, {"CurveForm", CurveForm}, {"ClosedCurve", ClosedCurve}, {"SelfIntersect", SelfIntersect}, {"KnotMultiplicities", KnotMultiplicities}, {"Knots", Knots}, {"KnotSpec", KnotSpec}}); }
inline Value IfcBSplineSurface(Value UDegree, Value VDegree, Value ControlPointsList, Value SurfaceForm, Value UClosed, Value VClosed, Value SelfIntersect) { return ifcapi::express::make_entity("IFC4X1", "IfcBSplineSurface", {{"UDegree", UDegree}, {"VDegree", VDegree}, {"ControlPointsList", ControlPointsList}, {"SurfaceForm", SurfaceForm}, {"UClosed", UClosed}, {"VClosed", VClosed}, {"SelfIntersect", SelfIntersect}}); }
inline Value IfcBSplineSurfaceWithKnots(Value UDegree, Value VDegree, Value ControlPointsList, Value SurfaceForm, Value UClosed, Value VClosed, Value SelfIntersect, Value UMultiplicities, Value VMultiplicities, Value UKnots, Value VKnots, Value KnotSpec) { return ifcapi::express::make_entity("IFC4X1", "IfcBSplineSurfaceWithKnots", {{"UDegree", UDegree}, {"VDegree", VDegree}, {"ControlPointsList", ControlPointsList}, {"SurfaceForm", SurfaceForm}, {"UClosed", UClosed}, {"VClosed", VClosed}, {"SelfIntersect", SelfIntersect}, {"UMultiplicities", UMultiplicities}, {"VMultiplicities", VMultiplicities}, {"UKnots", UKnots}, {"VKnots", VKnots}, {"KnotSpec", KnotSpec}}); }
inline Value IfcBeam(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcBeam", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcBeamStandardCase(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcBeamStandardCase", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcBeamType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcBeamType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcBlobTexture(Value RepeatS, Value RepeatT, Value Mode, Value TextureTransform, Value Parameter, Value RasterFormat, Value RasterCode) { return ifcapi::express::make_entity("IFC4X1", "IfcBlobTexture", {{"RepeatS", RepeatS}, {"RepeatT", RepeatT}, {"Mode", Mode}, {"TextureTransform", TextureTransform}, {"Parameter", Parameter}, {"RasterFormat", RasterFormat}, {"RasterCode", RasterCode}}); }
inline Value IfcBlock(Value Position, Value XLength, Value YLength, Value ZLength) { return ifcapi::express::make_entity("IFC4X1", "IfcBlock", {{"Position", Position}, {"XLength", XLength}, {"YLength", YLength}, {"ZLength", ZLength}}); }
inline Value IfcBoiler(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcBoiler", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcBoilerType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcBoilerType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcBooleanClippingResult(Value Operator_, Value FirstOperand, Value SecondOperand) { return ifcapi::express::make_entity("IFC4X1", "IfcBooleanClippingResult", {{"Operator", Operator_}, {"FirstOperand", FirstOperand}, {"SecondOperand", SecondOperand}}); }
inline Value IfcBooleanResult(Value Operator_, Value FirstOperand, Value SecondOperand) { return ifcapi::express::make_entity("IFC4X1", "IfcBooleanResult", {{"Operator", Operator_}, {"FirstOperand", FirstOperand}, {"SecondOperand", SecondOperand}}); }
inline Value IfcBoundaryCondition(Value Name) { return ifcapi::express::make_entity("IFC4X1", "IfcBoundaryCondition", {{"Name", Name}}); }
inline Value IfcBoundaryCurve(Value Segments, Value SelfIntersect) { return ifcapi::express::make_entity("IFC4X1", "IfcBoundaryCurve", {{"Segments", Segments}, {"SelfIntersect", SelfIntersect}}); }
inline Value IfcBoundaryEdgeCondition(Value Name, Value TranslationalStiffnessByLengthX, Value TranslationalStiffnessByLengthY, Value TranslationalStiffnessByLengthZ, Value RotationalStiffnessByLengthX, Value RotationalStiffnessByLengthY, Value RotationalStiffnessByLengthZ) { return ifcapi::express::make_entity("IFC4X1", "IfcBoundaryEdgeCondition", {{"Name", Name}, {"TranslationalStiffnessByLengthX", TranslationalStiffnessByLengthX}, {"TranslationalStiffnessByLengthY", TranslationalStiffnessByLengthY}, {"TranslationalStiffnessByLengthZ", TranslationalStiffnessByLengthZ}, {"RotationalStiffnessByLengthX", RotationalStiffnessByLengthX}, {"RotationalStiffnessByLengthY", RotationalStiffnessByLengthY}, {"RotationalStiffnessByLengthZ", RotationalStiffnessByLengthZ}}); }
inline Value IfcBoundaryFaceCondition(Value Name, Value TranslationalStiffnessByAreaX, Value TranslationalStiffnessByAreaY, Value TranslationalStiffnessByAreaZ) { return ifcapi::express::make_entity("IFC4X1", "IfcBoundaryFaceCondition", {{"Name", Name}, {"TranslationalStiffnessByAreaX", TranslationalStiffnessByAreaX}, {"TranslationalStiffnessByAreaY", TranslationalStiffnessByAreaY}, {"TranslationalStiffnessByAreaZ", TranslationalStiffnessByAreaZ}}); }
inline Value IfcBoundaryNodeCondition(Value Name, Value TranslationalStiffnessX, Value TranslationalStiffnessY, Value TranslationalStiffnessZ, Value RotationalStiffnessX, Value RotationalStiffnessY, Value RotationalStiffnessZ) { return ifcapi::express::make_entity("IFC4X1", "IfcBoundaryNodeCondition", {{"Name", Name}, {"TranslationalStiffnessX", TranslationalStiffnessX}, {"TranslationalStiffnessY", TranslationalStiffnessY}, {"TranslationalStiffnessZ", TranslationalStiffnessZ}, {"RotationalStiffnessX", RotationalStiffnessX}, {"RotationalStiffnessY", RotationalStiffnessY}, {"RotationalStiffnessZ", RotationalStiffnessZ}}); }
inline Value IfcBoundaryNodeConditionWarping(Value Name, Value TranslationalStiffnessX, Value TranslationalStiffnessY, Value TranslationalStiffnessZ, Value RotationalStiffnessX, Value RotationalStiffnessY, Value RotationalStiffnessZ, Value WarpingStiffness) { return ifcapi::express::make_entity("IFC4X1", "IfcBoundaryNodeConditionWarping", {{"Name", Name}, {"TranslationalStiffnessX", TranslationalStiffnessX}, {"TranslationalStiffnessY", TranslationalStiffnessY}, {"TranslationalStiffnessZ", TranslationalStiffnessZ}, {"RotationalStiffnessX", RotationalStiffnessX}, {"RotationalStiffnessY", RotationalStiffnessY}, {"RotationalStiffnessZ", RotationalStiffnessZ}, {"WarpingStiffness", WarpingStiffness}}); }
inline Value IfcBoundedCurve() { return ifcapi::express::make_entity("IFC4X1", "IfcBoundedCurve", {}); }
inline Value IfcBoundedSurface() { return ifcapi::express::make_entity("IFC4X1", "IfcBoundedSurface", {}); }
inline Value IfcBoundingBox(Value Corner, Value XDim, Value YDim, Value ZDim) { return ifcapi::express::make_entity("IFC4X1", "IfcBoundingBox", {{"Corner", Corner}, {"XDim", XDim}, {"YDim", YDim}, {"ZDim", ZDim}}); }
inline Value IfcBoxedHalfSpace(Value BaseSurface, Value AgreementFlag, Value Enclosure) { return ifcapi::express::make_entity("IFC4X1", "IfcBoxedHalfSpace", {{"BaseSurface", BaseSurface}, {"AgreementFlag", AgreementFlag}, {"Enclosure", Enclosure}}); }
inline Value IfcBuilding(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value LongName, Value CompositionType, Value ElevationOfRefHeight, Value ElevationOfTerrain, Value BuildingAddress) { return ifcapi::express::make_entity("IFC4X1", "IfcBuilding", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"LongName", LongName}, {"CompositionType", CompositionType}, {"ElevationOfRefHeight", ElevationOfRefHeight}, {"ElevationOfTerrain", ElevationOfTerrain}, {"BuildingAddress", BuildingAddress}}); }
inline Value IfcBuildingElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC4X1", "IfcBuildingElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcBuildingElementPart(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcBuildingElementPart", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcBuildingElementPartType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcBuildingElementPartType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcBuildingElementProxy(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcBuildingElementProxy", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcBuildingElementProxyType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcBuildingElementProxyType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcBuildingElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC4X1", "IfcBuildingElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcBuildingStorey(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value LongName, Value CompositionType, Value Elevation) { return ifcapi::express::make_entity("IFC4X1", "IfcBuildingStorey", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"LongName", LongName}, {"CompositionType", CompositionType}, {"Elevation", Elevation}}); }
inline Value IfcBuildingSystem(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value PredefinedType, Value LongName) { return ifcapi::express::make_entity("IFC4X1", "IfcBuildingSystem", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"PredefinedType", PredefinedType}, {"LongName", LongName}}); }
inline Value IfcBurner(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcBurner", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcBurnerType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcBurnerType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCShapeProfileDef(Value ProfileType, Value ProfileName, Value Position, Value Depth, Value Width, Value WallThickness, Value Girth, Value InternalFilletRadius) { return ifcapi::express::make_entity("IFC4X1", "IfcCShapeProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"Depth", Depth}, {"Width", Width}, {"WallThickness", WallThickness}, {"Girth", Girth}, {"InternalFilletRadius", InternalFilletRadius}}); }
inline Value IfcCableCarrierFitting(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCableCarrierFitting", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCableCarrierFittingType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCableCarrierFittingType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCableCarrierSegment(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCableCarrierSegment", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCableCarrierSegmentType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCableCarrierSegmentType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCableFitting(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCableFitting", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCableFittingType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCableFittingType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCableSegment(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCableSegment", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCableSegmentType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCableSegmentType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCartesianPoint(Value Coordinates) { return ifcapi::express::make_entity("IFC4X1", "IfcCartesianPoint", {{"Coordinates", Coordinates}}); }
inline Value IfcCartesianPointList() { return ifcapi::express::make_entity("IFC4X1", "IfcCartesianPointList", {}); }
inline Value IfcCartesianPointList2D(Value CoordList, Value TagList) { return ifcapi::express::make_entity("IFC4X1", "IfcCartesianPointList2D", {{"CoordList", CoordList}, {"TagList", TagList}}); }
inline Value IfcCartesianPointList3D(Value CoordList, Value TagList) { return ifcapi::express::make_entity("IFC4X1", "IfcCartesianPointList3D", {{"CoordList", CoordList}, {"TagList", TagList}}); }
inline Value IfcCartesianTransformationOperator(Value Axis1, Value Axis2, Value LocalOrigin, Value Scale) { return ifcapi::express::make_entity("IFC4X1", "IfcCartesianTransformationOperator", {{"Axis1", Axis1}, {"Axis2", Axis2}, {"LocalOrigin", LocalOrigin}, {"Scale", Scale}}); }
inline Value IfcCartesianTransformationOperator2D(Value Axis1, Value Axis2, Value LocalOrigin, Value Scale) { return ifcapi::express::make_entity("IFC4X1", "IfcCartesianTransformationOperator2D", {{"Axis1", Axis1}, {"Axis2", Axis2}, {"LocalOrigin", LocalOrigin}, {"Scale", Scale}}); }
inline Value IfcCartesianTransformationOperator2DnonUniform(Value Axis1, Value Axis2, Value LocalOrigin, Value Scale, Value Scale2) { return ifcapi::express::make_entity("IFC4X1", "IfcCartesianTransformationOperator2DnonUniform", {{"Axis1", Axis1}, {"Axis2", Axis2}, {"LocalOrigin", LocalOrigin}, {"Scale", Scale}, {"Scale2", Scale2}}); }
inline Value IfcCartesianTransformationOperator3D(Value Axis1, Value Axis2, Value LocalOrigin, Value Scale, Value Axis3) { return ifcapi::express::make_entity("IFC4X1", "IfcCartesianTransformationOperator3D", {{"Axis1", Axis1}, {"Axis2", Axis2}, {"LocalOrigin", LocalOrigin}, {"Scale", Scale}, {"Axis3", Axis3}}); }
inline Value IfcCartesianTransformationOperator3DnonUniform(Value Axis1, Value Axis2, Value LocalOrigin, Value Scale, Value Axis3, Value Scale2, Value Scale3) { return ifcapi::express::make_entity("IFC4X1", "IfcCartesianTransformationOperator3DnonUniform", {{"Axis1", Axis1}, {"Axis2", Axis2}, {"LocalOrigin", LocalOrigin}, {"Scale", Scale}, {"Axis3", Axis3}, {"Scale2", Scale2}, {"Scale3", Scale3}}); }
inline Value IfcCenterLineProfileDef(Value ProfileType, Value ProfileName, Value Curve, Value Thickness) { return ifcapi::express::make_entity("IFC4X1", "IfcCenterLineProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Curve", Curve}, {"Thickness", Thickness}}); }
inline Value IfcChiller(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcChiller", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcChillerType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcChillerType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcChimney(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcChimney", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcChimneyType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcChimneyType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCircle(Value Position, Value Radius) { return ifcapi::express::make_entity("IFC4X1", "IfcCircle", {{"Position", Position}, {"Radius", Radius}}); }
inline Value IfcCircleHollowProfileDef(Value ProfileType, Value ProfileName, Value Position, Value Radius, Value WallThickness) { return ifcapi::express::make_entity("IFC4X1", "IfcCircleHollowProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"Radius", Radius}, {"WallThickness", WallThickness}}); }
inline Value IfcCircleProfileDef(Value ProfileType, Value ProfileName, Value Position, Value Radius) { return ifcapi::express::make_entity("IFC4X1", "IfcCircleProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"Radius", Radius}}); }
inline Value IfcCircularArcSegment2D(Value StartPoint, Value StartDirection, Value SegmentLength, Value Radius, Value IsCCW) { return ifcapi::express::make_entity("IFC4X1", "IfcCircularArcSegment2D", {{"StartPoint", StartPoint}, {"StartDirection", StartDirection}, {"SegmentLength", SegmentLength}, {"Radius", Radius}, {"IsCCW", IsCCW}}); }
inline Value IfcCivilElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC4X1", "IfcCivilElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcCivilElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC4X1", "IfcCivilElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcClassification(Value Source, Value Edition, Value EditionDate, Value Name, Value Description, Value Location, Value ReferenceTokens) { return ifcapi::express::make_entity("IFC4X1", "IfcClassification", {{"Source", Source}, {"Edition", Edition}, {"EditionDate", EditionDate}, {"Name", Name}, {"Description", Description}, {"Location", Location}, {"ReferenceTokens", ReferenceTokens}}); }
inline Value IfcClassificationReference(Value Location, Value Identification, Value Name, Value ReferencedSource, Value Description, Value Sort) { return ifcapi::express::make_entity("IFC4X1", "IfcClassificationReference", {{"Location", Location}, {"Identification", Identification}, {"Name", Name}, {"ReferencedSource", ReferencedSource}, {"Description", Description}, {"Sort", Sort}}); }
inline Value IfcClosedShell(Value CfsFaces) { return ifcapi::express::make_entity("IFC4X1", "IfcClosedShell", {{"CfsFaces", CfsFaces}}); }
inline Value IfcCoil(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCoil", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCoilType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCoilType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcColourRgb(Value Name, Value Red, Value Green, Value Blue) { return ifcapi::express::make_entity("IFC4X1", "IfcColourRgb", {{"Name", Name}, {"Red", Red}, {"Green", Green}, {"Blue", Blue}}); }
inline Value IfcColourRgbList(Value ColourList) { return ifcapi::express::make_entity("IFC4X1", "IfcColourRgbList", {{"ColourList", ColourList}}); }
inline Value IfcColourSpecification(Value Name) { return ifcapi::express::make_entity("IFC4X1", "IfcColourSpecification", {{"Name", Name}}); }
inline Value IfcColumn(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcColumn", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcColumnStandardCase(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcColumnStandardCase", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcColumnType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcColumnType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCommunicationsAppliance(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCommunicationsAppliance", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCommunicationsApplianceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCommunicationsApplianceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcComplexProperty(Value Name, Value Description, Value UsageName, Value HasProperties) { return ifcapi::express::make_entity("IFC4X1", "IfcComplexProperty", {{"Name", Name}, {"Description", Description}, {"UsageName", UsageName}, {"HasProperties", HasProperties}}); }
inline Value IfcComplexPropertyTemplate(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value UsageName, Value TemplateType, Value HasPropertyTemplates) { return ifcapi::express::make_entity("IFC4X1", "IfcComplexPropertyTemplate", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"UsageName", UsageName}, {"TemplateType", TemplateType}, {"HasPropertyTemplates", HasPropertyTemplates}}); }
inline Value IfcCompositeCurve(Value Segments, Value SelfIntersect) { return ifcapi::express::make_entity("IFC4X1", "IfcCompositeCurve", {{"Segments", Segments}, {"SelfIntersect", SelfIntersect}}); }
inline Value IfcCompositeCurveOnSurface(Value Segments, Value SelfIntersect) { return ifcapi::express::make_entity("IFC4X1", "IfcCompositeCurveOnSurface", {{"Segments", Segments}, {"SelfIntersect", SelfIntersect}}); }
inline Value IfcCompositeCurveSegment(Value Transition, Value SameSense, Value ParentCurve) { return ifcapi::express::make_entity("IFC4X1", "IfcCompositeCurveSegment", {{"Transition", Transition}, {"SameSense", SameSense}, {"ParentCurve", ParentCurve}}); }
inline Value IfcCompositeProfileDef(Value ProfileType, Value ProfileName, Value Profiles, Value Label) { return ifcapi::express::make_entity("IFC4X1", "IfcCompositeProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Profiles", Profiles}, {"Label", Label}}); }
inline Value IfcCompressor(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCompressor", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCompressorType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCompressorType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCondenser(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCondenser", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCondenserType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCondenserType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcConic(Value Position) { return ifcapi::express::make_entity("IFC4X1", "IfcConic", {{"Position", Position}}); }
inline Value IfcConnectedFaceSet(Value CfsFaces) { return ifcapi::express::make_entity("IFC4X1", "IfcConnectedFaceSet", {{"CfsFaces", CfsFaces}}); }
inline Value IfcConnectionCurveGeometry(Value CurveOnRelatingElement, Value CurveOnRelatedElement) { return ifcapi::express::make_entity("IFC4X1", "IfcConnectionCurveGeometry", {{"CurveOnRelatingElement", CurveOnRelatingElement}, {"CurveOnRelatedElement", CurveOnRelatedElement}}); }
inline Value IfcConnectionGeometry() { return ifcapi::express::make_entity("IFC4X1", "IfcConnectionGeometry", {}); }
inline Value IfcConnectionPointEccentricity(Value PointOnRelatingElement, Value PointOnRelatedElement, Value EccentricityInX, Value EccentricityInY, Value EccentricityInZ) { return ifcapi::express::make_entity("IFC4X1", "IfcConnectionPointEccentricity", {{"PointOnRelatingElement", PointOnRelatingElement}, {"PointOnRelatedElement", PointOnRelatedElement}, {"EccentricityInX", EccentricityInX}, {"EccentricityInY", EccentricityInY}, {"EccentricityInZ", EccentricityInZ}}); }
inline Value IfcConnectionPointGeometry(Value PointOnRelatingElement, Value PointOnRelatedElement) { return ifcapi::express::make_entity("IFC4X1", "IfcConnectionPointGeometry", {{"PointOnRelatingElement", PointOnRelatingElement}, {"PointOnRelatedElement", PointOnRelatedElement}}); }
inline Value IfcConnectionSurfaceGeometry(Value SurfaceOnRelatingElement, Value SurfaceOnRelatedElement) { return ifcapi::express::make_entity("IFC4X1", "IfcConnectionSurfaceGeometry", {{"SurfaceOnRelatingElement", SurfaceOnRelatingElement}, {"SurfaceOnRelatedElement", SurfaceOnRelatedElement}}); }
inline Value IfcConnectionVolumeGeometry(Value VolumeOnRelatingElement, Value VolumeOnRelatedElement) { return ifcapi::express::make_entity("IFC4X1", "IfcConnectionVolumeGeometry", {{"VolumeOnRelatingElement", VolumeOnRelatingElement}, {"VolumeOnRelatedElement", VolumeOnRelatedElement}}); }
inline Value IfcConstraint(Value Name, Value Description, Value ConstraintGrade, Value ConstraintSource, Value CreatingActor, Value CreationTime, Value UserDefinedGrade) { return ifcapi::express::make_entity("IFC4X1", "IfcConstraint", {{"Name", Name}, {"Description", Description}, {"ConstraintGrade", ConstraintGrade}, {"ConstraintSource", ConstraintSource}, {"CreatingActor", CreatingActor}, {"CreationTime", CreationTime}, {"UserDefinedGrade", UserDefinedGrade}}); }
inline Value IfcConstructionEquipmentResource(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identification, Value LongDescription, Value Usage, Value BaseCosts, Value BaseQuantity, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcConstructionEquipmentResource", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identification", Identification}, {"LongDescription", LongDescription}, {"Usage", Usage}, {"BaseCosts", BaseCosts}, {"BaseQuantity", BaseQuantity}, {"PredefinedType", PredefinedType}}); }
inline Value IfcConstructionEquipmentResourceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value Identification, Value LongDescription, Value ResourceType, Value BaseCosts, Value BaseQuantity, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcConstructionEquipmentResourceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"Identification", Identification}, {"LongDescription", LongDescription}, {"ResourceType", ResourceType}, {"BaseCosts", BaseCosts}, {"BaseQuantity", BaseQuantity}, {"PredefinedType", PredefinedType}}); }
inline Value IfcConstructionMaterialResource(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identification, Value LongDescription, Value Usage, Value BaseCosts, Value BaseQuantity, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcConstructionMaterialResource", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identification", Identification}, {"LongDescription", LongDescription}, {"Usage", Usage}, {"BaseCosts", BaseCosts}, {"BaseQuantity", BaseQuantity}, {"PredefinedType", PredefinedType}}); }
inline Value IfcConstructionMaterialResourceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value Identification, Value LongDescription, Value ResourceType, Value BaseCosts, Value BaseQuantity, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcConstructionMaterialResourceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"Identification", Identification}, {"LongDescription", LongDescription}, {"ResourceType", ResourceType}, {"BaseCosts", BaseCosts}, {"BaseQuantity", BaseQuantity}, {"PredefinedType", PredefinedType}}); }
inline Value IfcConstructionProductResource(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identification, Value LongDescription, Value Usage, Value BaseCosts, Value BaseQuantity, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcConstructionProductResource", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identification", Identification}, {"LongDescription", LongDescription}, {"Usage", Usage}, {"BaseCosts", BaseCosts}, {"BaseQuantity", BaseQuantity}, {"PredefinedType", PredefinedType}}); }
inline Value IfcConstructionProductResourceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value Identification, Value LongDescription, Value ResourceType, Value BaseCosts, Value BaseQuantity, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcConstructionProductResourceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"Identification", Identification}, {"LongDescription", LongDescription}, {"ResourceType", ResourceType}, {"BaseCosts", BaseCosts}, {"BaseQuantity", BaseQuantity}, {"PredefinedType", PredefinedType}}); }
inline Value IfcConstructionResource(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identification, Value LongDescription, Value Usage, Value BaseCosts, Value BaseQuantity) { return ifcapi::express::make_entity("IFC4X1", "IfcConstructionResource", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identification", Identification}, {"LongDescription", LongDescription}, {"Usage", Usage}, {"BaseCosts", BaseCosts}, {"BaseQuantity", BaseQuantity}}); }
inline Value IfcConstructionResourceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value Identification, Value LongDescription, Value ResourceType, Value BaseCosts, Value BaseQuantity) { return ifcapi::express::make_entity("IFC4X1", "IfcConstructionResourceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"Identification", Identification}, {"LongDescription", LongDescription}, {"ResourceType", ResourceType}, {"BaseCosts", BaseCosts}, {"BaseQuantity", BaseQuantity}}); }
inline Value IfcContext(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value LongName, Value Phase, Value RepresentationContexts, Value UnitsInContext) { return ifcapi::express::make_entity("IFC4X1", "IfcContext", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"LongName", LongName}, {"Phase", Phase}, {"RepresentationContexts", RepresentationContexts}, {"UnitsInContext", UnitsInContext}}); }
inline Value IfcContextDependentUnit(Value Dimensions, Value UnitType, Value Name) { return ifcapi::express::make_entity("IFC4X1", "IfcContextDependentUnit", {{"Dimensions", Dimensions}, {"UnitType", UnitType}, {"Name", Name}}); }
inline Value IfcControl(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identification) { return ifcapi::express::make_entity("IFC4X1", "IfcControl", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identification", Identification}}); }
inline Value IfcController(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcController", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcControllerType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcControllerType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcConversionBasedUnit(Value Dimensions, Value UnitType, Value Name, Value ConversionFactor) { return ifcapi::express::make_entity("IFC4X1", "IfcConversionBasedUnit", {{"Dimensions", Dimensions}, {"UnitType", UnitType}, {"Name", Name}, {"ConversionFactor", ConversionFactor}}); }
inline Value IfcConversionBasedUnitWithOffset(Value Dimensions, Value UnitType, Value Name, Value ConversionFactor, Value ConversionOffset) { return ifcapi::express::make_entity("IFC4X1", "IfcConversionBasedUnitWithOffset", {{"Dimensions", Dimensions}, {"UnitType", UnitType}, {"Name", Name}, {"ConversionFactor", ConversionFactor}, {"ConversionOffset", ConversionOffset}}); }
inline Value IfcCooledBeam(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCooledBeam", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCooledBeamType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCooledBeamType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCoolingTower(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCoolingTower", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCoolingTowerType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCoolingTowerType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCoordinateOperation(Value SourceCRS, Value TargetCRS) { return ifcapi::express::make_entity("IFC4X1", "IfcCoordinateOperation", {{"SourceCRS", SourceCRS}, {"TargetCRS", TargetCRS}}); }
inline Value IfcCoordinateReferenceSystem(Value Name, Value Description, Value GeodeticDatum, Value VerticalDatum) { return ifcapi::express::make_entity("IFC4X1", "IfcCoordinateReferenceSystem", {{"Name", Name}, {"Description", Description}, {"GeodeticDatum", GeodeticDatum}, {"VerticalDatum", VerticalDatum}}); }
inline Value IfcCostItem(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identification, Value PredefinedType, Value CostValues, Value CostQuantities) { return ifcapi::express::make_entity("IFC4X1", "IfcCostItem", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identification", Identification}, {"PredefinedType", PredefinedType}, {"CostValues", CostValues}, {"CostQuantities", CostQuantities}}); }
inline Value IfcCostSchedule(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identification, Value PredefinedType, Value Status, Value SubmittedOn, Value UpdateDate) { return ifcapi::express::make_entity("IFC4X1", "IfcCostSchedule", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identification", Identification}, {"PredefinedType", PredefinedType}, {"Status", Status}, {"SubmittedOn", SubmittedOn}, {"UpdateDate", UpdateDate}}); }
inline Value IfcCostValue(Value Name, Value Description, Value AppliedValue, Value UnitBasis, Value ApplicableDate, Value FixedUntilDate, Value Category, Value Condition, Value ArithmeticOperator, Value Components) { return ifcapi::express::make_entity("IFC4X1", "IfcCostValue", {{"Name", Name}, {"Description", Description}, {"AppliedValue", AppliedValue}, {"UnitBasis", UnitBasis}, {"ApplicableDate", ApplicableDate}, {"FixedUntilDate", FixedUntilDate}, {"Category", Category}, {"Condition", Condition}, {"ArithmeticOperator", ArithmeticOperator}, {"Components", Components}}); }
inline Value IfcCovering(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCovering", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCoveringType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCoveringType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCrewResource(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identification, Value LongDescription, Value Usage, Value BaseCosts, Value BaseQuantity, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCrewResource", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identification", Identification}, {"LongDescription", LongDescription}, {"Usage", Usage}, {"BaseCosts", BaseCosts}, {"BaseQuantity", BaseQuantity}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCrewResourceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value Identification, Value LongDescription, Value ResourceType, Value BaseCosts, Value BaseQuantity, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCrewResourceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"Identification", Identification}, {"LongDescription", LongDescription}, {"ResourceType", ResourceType}, {"BaseCosts", BaseCosts}, {"BaseQuantity", BaseQuantity}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCsgPrimitive3D(Value Position) { return ifcapi::express::make_entity("IFC4X1", "IfcCsgPrimitive3D", {{"Position", Position}}); }
inline Value IfcCsgSolid(Value TreeRootExpression) { return ifcapi::express::make_entity("IFC4X1", "IfcCsgSolid", {{"TreeRootExpression", TreeRootExpression}}); }
inline Value IfcCurrencyRelationship(Value Name, Value Description, Value RelatingMonetaryUnit, Value RelatedMonetaryUnit, Value ExchangeRate, Value RateDateTime, Value RateSource) { return ifcapi::express::make_entity("IFC4X1", "IfcCurrencyRelationship", {{"Name", Name}, {"Description", Description}, {"RelatingMonetaryUnit", RelatingMonetaryUnit}, {"RelatedMonetaryUnit", RelatedMonetaryUnit}, {"ExchangeRate", ExchangeRate}, {"RateDateTime", RateDateTime}, {"RateSource", RateSource}}); }
inline Value IfcCurtainWall(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCurtainWall", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCurtainWallType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcCurtainWallType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCurve() { return ifcapi::express::make_entity("IFC4X1", "IfcCurve", {}); }
inline Value IfcCurveBoundedPlane(Value BasisSurface, Value OuterBoundary, Value InnerBoundaries) { return ifcapi::express::make_entity("IFC4X1", "IfcCurveBoundedPlane", {{"BasisSurface", BasisSurface}, {"OuterBoundary", OuterBoundary}, {"InnerBoundaries", InnerBoundaries}}); }
inline Value IfcCurveBoundedSurface(Value BasisSurface, Value Boundaries, Value ImplicitOuter) { return ifcapi::express::make_entity("IFC4X1", "IfcCurveBoundedSurface", {{"BasisSurface", BasisSurface}, {"Boundaries", Boundaries}, {"ImplicitOuter", ImplicitOuter}}); }
inline Value IfcCurveSegment2D(Value StartPoint, Value StartDirection, Value SegmentLength) { return ifcapi::express::make_entity("IFC4X1", "IfcCurveSegment2D", {{"StartPoint", StartPoint}, {"StartDirection", StartDirection}, {"SegmentLength", SegmentLength}}); }
inline Value IfcCurveStyle(Value Name, Value CurveFont, Value CurveWidth, Value CurveColour, Value ModelOrDraughting) { return ifcapi::express::make_entity("IFC4X1", "IfcCurveStyle", {{"Name", Name}, {"CurveFont", CurveFont}, {"CurveWidth", CurveWidth}, {"CurveColour", CurveColour}, {"ModelOrDraughting", ModelOrDraughting}}); }
inline Value IfcCurveStyleFont(Value Name, Value PatternList) { return ifcapi::express::make_entity("IFC4X1", "IfcCurveStyleFont", {{"Name", Name}, {"PatternList", PatternList}}); }
inline Value IfcCurveStyleFontAndScaling(Value Name, Value CurveFont, Value CurveFontScaling) { return ifcapi::express::make_entity("IFC4X1", "IfcCurveStyleFontAndScaling", {{"Name", Name}, {"CurveFont", CurveFont}, {"CurveFontScaling", CurveFontScaling}}); }
inline Value IfcCurveStyleFontPattern(Value VisibleSegmentLength, Value InvisibleSegmentLength) { return ifcapi::express::make_entity("IFC4X1", "IfcCurveStyleFontPattern", {{"VisibleSegmentLength", VisibleSegmentLength}, {"InvisibleSegmentLength", InvisibleSegmentLength}}); }
inline Value IfcCylindricalSurface(Value Position, Value Radius) { return ifcapi::express::make_entity("IFC4X1", "IfcCylindricalSurface", {{"Position", Position}, {"Radius", Radius}}); }
inline Value IfcDamper(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcDamper", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcDamperType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcDamperType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcDerivedProfileDef(Value ProfileType, Value ProfileName, Value ParentProfile, Value Operator_, Value Label) { return ifcapi::express::make_entity("IFC4X1", "IfcDerivedProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"ParentProfile", ParentProfile}, {"Operator", Operator_}, {"Label", Label}}); }
inline Value IfcDerivedUnit(Value Elements, Value UnitType, Value UserDefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcDerivedUnit", {{"Elements", Elements}, {"UnitType", UnitType}, {"UserDefinedType", UserDefinedType}}); }
inline Value IfcDerivedUnitElement(Value Unit, Value Exponent) { return ifcapi::express::make_entity("IFC4X1", "IfcDerivedUnitElement", {{"Unit", Unit}, {"Exponent", Exponent}}); }
inline Value IfcDimensionalExponents(Value LengthExponent, Value MassExponent, Value TimeExponent, Value ElectricCurrentExponent, Value ThermodynamicTemperatureExponent, Value AmountOfSubstanceExponent, Value LuminousIntensityExponent) { return ifcapi::express::make_entity("IFC4X1", "IfcDimensionalExponents", {{"LengthExponent", LengthExponent}, {"MassExponent", MassExponent}, {"TimeExponent", TimeExponent}, {"ElectricCurrentExponent", ElectricCurrentExponent}, {"ThermodynamicTemperatureExponent", ThermodynamicTemperatureExponent}, {"AmountOfSubstanceExponent", AmountOfSubstanceExponent}, {"LuminousIntensityExponent", LuminousIntensityExponent}}); }
inline Value IfcDirection(Value DirectionRatios) { return ifcapi::express::make_entity("IFC4X1", "IfcDirection", {{"DirectionRatios", DirectionRatios}}); }
inline Value IfcDiscreteAccessory(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcDiscreteAccessory", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcDiscreteAccessoryType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcDiscreteAccessoryType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcDistanceExpression(Value DistanceAlong, Value OffsetLateral, Value OffsetVertical, Value OffsetLongitudinal, Value AlongHorizontal) { return ifcapi::express::make_entity("IFC4X1", "IfcDistanceExpression", {{"DistanceAlong", DistanceAlong}, {"OffsetLateral", OffsetLateral}, {"OffsetVertical", OffsetVertical}, {"OffsetLongitudinal", OffsetLongitudinal}, {"AlongHorizontal", AlongHorizontal}}); }
inline Value IfcDistributionChamberElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcDistributionChamberElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcDistributionChamberElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcDistributionChamberElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcDistributionCircuit(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value LongName, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcDistributionCircuit", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"LongName", LongName}, {"PredefinedType", PredefinedType}}); }
inline Value IfcDistributionControlElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC4X1", "IfcDistributionControlElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcDistributionControlElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC4X1", "IfcDistributionControlElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcDistributionElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC4X1", "IfcDistributionElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcDistributionElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC4X1", "IfcDistributionElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcDistributionFlowElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC4X1", "IfcDistributionFlowElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcDistributionFlowElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC4X1", "IfcDistributionFlowElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcDistributionPort(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value FlowDirection, Value PredefinedType, Value SystemType) { return ifcapi::express::make_entity("IFC4X1", "IfcDistributionPort", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"FlowDirection", FlowDirection}, {"PredefinedType", PredefinedType}, {"SystemType", SystemType}}); }
inline Value IfcDistributionSystem(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value LongName, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcDistributionSystem", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"LongName", LongName}, {"PredefinedType", PredefinedType}}); }
inline Value IfcDocumentInformation(Value Identification, Value Name, Value Description, Value Location, Value Purpose, Value IntendedUse, Value Scope, Value Revision, Value DocumentOwner, Value Editors, Value CreationTime, Value LastRevisionTime, Value ElectronicFormat, Value ValidFrom, Value ValidUntil, Value Confidentiality, Value Status) { return ifcapi::express::make_entity("IFC4X1", "IfcDocumentInformation", {{"Identification", Identification}, {"Name", Name}, {"Description", Description}, {"Location", Location}, {"Purpose", Purpose}, {"IntendedUse", IntendedUse}, {"Scope", Scope}, {"Revision", Revision}, {"DocumentOwner", DocumentOwner}, {"Editors", Editors}, {"CreationTime", CreationTime}, {"LastRevisionTime", LastRevisionTime}, {"ElectronicFormat", ElectronicFormat}, {"ValidFrom", ValidFrom}, {"ValidUntil", ValidUntil}, {"Confidentiality", Confidentiality}, {"Status", Status}}); }
inline Value IfcDocumentInformationRelationship(Value Name, Value Description, Value RelatingDocument, Value RelatedDocuments, Value RelationshipType) { return ifcapi::express::make_entity("IFC4X1", "IfcDocumentInformationRelationship", {{"Name", Name}, {"Description", Description}, {"RelatingDocument", RelatingDocument}, {"RelatedDocuments", RelatedDocuments}, {"RelationshipType", RelationshipType}}); }
inline Value IfcDocumentReference(Value Location, Value Identification, Value Name, Value Description, Value ReferencedDocument) { return ifcapi::express::make_entity("IFC4X1", "IfcDocumentReference", {{"Location", Location}, {"Identification", Identification}, {"Name", Name}, {"Description", Description}, {"ReferencedDocument", ReferencedDocument}}); }
inline Value IfcDoor(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value OverallHeight, Value OverallWidth, Value PredefinedType, Value OperationType, Value UserDefinedOperationType) { return ifcapi::express::make_entity("IFC4X1", "IfcDoor", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"OverallHeight", OverallHeight}, {"OverallWidth", OverallWidth}, {"PredefinedType", PredefinedType}, {"OperationType", OperationType}, {"UserDefinedOperationType", UserDefinedOperationType}}); }
inline Value IfcDoorLiningProperties(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value LiningDepth, Value LiningThickness, Value ThresholdDepth, Value ThresholdThickness, Value TransomThickness, Value TransomOffset, Value LiningOffset, Value ThresholdOffset, Value CasingThickness, Value CasingDepth, Value ShapeAspectStyle, Value LiningToPanelOffsetX, Value LiningToPanelOffsetY) { return ifcapi::express::make_entity("IFC4X1", "IfcDoorLiningProperties", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"LiningDepth", LiningDepth}, {"LiningThickness", LiningThickness}, {"ThresholdDepth", ThresholdDepth}, {"ThresholdThickness", ThresholdThickness}, {"TransomThickness", TransomThickness}, {"TransomOffset", TransomOffset}, {"LiningOffset", LiningOffset}, {"ThresholdOffset", ThresholdOffset}, {"CasingThickness", CasingThickness}, {"CasingDepth", CasingDepth}, {"ShapeAspectStyle", ShapeAspectStyle}, {"LiningToPanelOffsetX", LiningToPanelOffsetX}, {"LiningToPanelOffsetY", LiningToPanelOffsetY}}); }
inline Value IfcDoorPanelProperties(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value PanelDepth, Value PanelOperation, Value PanelWidth, Value PanelPosition, Value ShapeAspectStyle) { return ifcapi::express::make_entity("IFC4X1", "IfcDoorPanelProperties", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"PanelDepth", PanelDepth}, {"PanelOperation", PanelOperation}, {"PanelWidth", PanelWidth}, {"PanelPosition", PanelPosition}, {"ShapeAspectStyle", ShapeAspectStyle}}); }
inline Value IfcDoorStandardCase(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value OverallHeight, Value OverallWidth, Value PredefinedType, Value OperationType, Value UserDefinedOperationType) { return ifcapi::express::make_entity("IFC4X1", "IfcDoorStandardCase", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"OverallHeight", OverallHeight}, {"OverallWidth", OverallWidth}, {"PredefinedType", PredefinedType}, {"OperationType", OperationType}, {"UserDefinedOperationType", UserDefinedOperationType}}); }
inline Value IfcDoorStyle(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value OperationType, Value ConstructionType, Value ParameterTakesPrecedence, Value Sizeable) { return ifcapi::express::make_entity("IFC4X1", "IfcDoorStyle", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"OperationType", OperationType}, {"ConstructionType", ConstructionType}, {"ParameterTakesPrecedence", ParameterTakesPrecedence}, {"Sizeable", Sizeable}}); }
inline Value IfcDoorType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType, Value OperationType, Value ParameterTakesPrecedence, Value UserDefinedOperationType) { return ifcapi::express::make_entity("IFC4X1", "IfcDoorType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}, {"OperationType", OperationType}, {"ParameterTakesPrecedence", ParameterTakesPrecedence}, {"UserDefinedOperationType", UserDefinedOperationType}}); }
inline Value IfcDraughtingPreDefinedColour(Value Name) { return ifcapi::express::make_entity("IFC4X1", "IfcDraughtingPreDefinedColour", {{"Name", Name}}); }
inline Value IfcDraughtingPreDefinedCurveFont(Value Name) { return ifcapi::express::make_entity("IFC4X1", "IfcDraughtingPreDefinedCurveFont", {{"Name", Name}}); }
inline Value IfcDuctFitting(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcDuctFitting", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcDuctFittingType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcDuctFittingType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcDuctSegment(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcDuctSegment", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcDuctSegmentType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcDuctSegmentType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcDuctSilencer(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcDuctSilencer", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcDuctSilencerType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcDuctSilencerType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcEdge(Value EdgeStart, Value EdgeEnd) { return ifcapi::express::make_entity("IFC4X1", "IfcEdge", {{"EdgeStart", EdgeStart}, {"EdgeEnd", EdgeEnd}}); }
inline Value IfcEdgeCurve(Value EdgeStart, Value EdgeEnd, Value EdgeGeometry, Value SameSense) { return ifcapi::express::make_entity("IFC4X1", "IfcEdgeCurve", {{"EdgeStart", EdgeStart}, {"EdgeEnd", EdgeEnd}, {"EdgeGeometry", EdgeGeometry}, {"SameSense", SameSense}}); }
inline Value IfcEdgeLoop(Value EdgeList) { return ifcapi::express::make_entity("IFC4X1", "IfcEdgeLoop", {{"EdgeList", EdgeList}}); }
inline Value IfcElectricAppliance(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcElectricAppliance", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcElectricApplianceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcElectricApplianceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcElectricDistributionBoard(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcElectricDistributionBoard", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcElectricDistributionBoardType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcElectricDistributionBoardType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcElectricFlowStorageDevice(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcElectricFlowStorageDevice", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcElectricFlowStorageDeviceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcElectricFlowStorageDeviceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcElectricGenerator(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcElectricGenerator", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcElectricGeneratorType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcElectricGeneratorType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcElectricMotor(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcElectricMotor", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcElectricMotorType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcElectricMotorType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcElectricTimeControl(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcElectricTimeControl", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcElectricTimeControlType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcElectricTimeControlType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC4X1", "IfcElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcElementAssembly(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value AssemblyPlace, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcElementAssembly", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"AssemblyPlace", AssemblyPlace}, {"PredefinedType", PredefinedType}}); }
inline Value IfcElementAssemblyType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcElementAssemblyType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcElementComponent(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC4X1", "IfcElementComponent", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcElementComponentType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC4X1", "IfcElementComponentType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcElementQuantity(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value MethodOfMeasurement, Value Quantities) { return ifcapi::express::make_entity("IFC4X1", "IfcElementQuantity", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"MethodOfMeasurement", MethodOfMeasurement}, {"Quantities", Quantities}}); }
inline Value IfcElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC4X1", "IfcElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcElementarySurface(Value Position) { return ifcapi::express::make_entity("IFC4X1", "IfcElementarySurface", {{"Position", Position}}); }
inline Value IfcEllipse(Value Position, Value SemiAxis1, Value SemiAxis2) { return ifcapi::express::make_entity("IFC4X1", "IfcEllipse", {{"Position", Position}, {"SemiAxis1", SemiAxis1}, {"SemiAxis2", SemiAxis2}}); }
inline Value IfcEllipseProfileDef(Value ProfileType, Value ProfileName, Value Position, Value SemiAxis1, Value SemiAxis2) { return ifcapi::express::make_entity("IFC4X1", "IfcEllipseProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"SemiAxis1", SemiAxis1}, {"SemiAxis2", SemiAxis2}}); }
inline Value IfcEnergyConversionDevice(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC4X1", "IfcEnergyConversionDevice", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcEnergyConversionDeviceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC4X1", "IfcEnergyConversionDeviceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcEngine(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcEngine", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcEngineType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcEngineType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcEvaporativeCooler(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcEvaporativeCooler", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcEvaporativeCoolerType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcEvaporativeCoolerType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcEvaporator(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcEvaporator", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcEvaporatorType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcEvaporatorType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcEvent(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identification, Value LongDescription, Value PredefinedType, Value EventTriggerType, Value UserDefinedEventTriggerType, Value EventOccurenceTime) { return ifcapi::express::make_entity("IFC4X1", "IfcEvent", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identification", Identification}, {"LongDescription", LongDescription}, {"PredefinedType", PredefinedType}, {"EventTriggerType", EventTriggerType}, {"UserDefinedEventTriggerType", UserDefinedEventTriggerType}, {"EventOccurenceTime", EventOccurenceTime}}); }
inline Value IfcEventTime(Value Name, Value DataOrigin, Value UserDefinedDataOrigin, Value ActualDate, Value EarlyDate, Value LateDate, Value ScheduleDate) { return ifcapi::express::make_entity("IFC4X1", "IfcEventTime", {{"Name", Name}, {"DataOrigin", DataOrigin}, {"UserDefinedDataOrigin", UserDefinedDataOrigin}, {"ActualDate", ActualDate}, {"EarlyDate", EarlyDate}, {"LateDate", LateDate}, {"ScheduleDate", ScheduleDate}}); }
inline Value IfcEventType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value Identification, Value LongDescription, Value ProcessType, Value PredefinedType, Value EventTriggerType, Value UserDefinedEventTriggerType) { return ifcapi::express::make_entity("IFC4X1", "IfcEventType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"Identification", Identification}, {"LongDescription", LongDescription}, {"ProcessType", ProcessType}, {"PredefinedType", PredefinedType}, {"EventTriggerType", EventTriggerType}, {"UserDefinedEventTriggerType", UserDefinedEventTriggerType}}); }
inline Value IfcExtendedProperties(Value Name, Value Description, Value Properties) { return ifcapi::express::make_entity("IFC4X1", "IfcExtendedProperties", {{"Name", Name}, {"Description", Description}, {"Properties", Properties}}); }
inline Value IfcExternalInformation() { return ifcapi::express::make_entity("IFC4X1", "IfcExternalInformation", {}); }
inline Value IfcExternalReference(Value Location, Value Identification, Value Name) { return ifcapi::express::make_entity("IFC4X1", "IfcExternalReference", {{"Location", Location}, {"Identification", Identification}, {"Name", Name}}); }
inline Value IfcExternalReferenceRelationship(Value Name, Value Description, Value RelatingReference, Value RelatedResourceObjects) { return ifcapi::express::make_entity("IFC4X1", "IfcExternalReferenceRelationship", {{"Name", Name}, {"Description", Description}, {"RelatingReference", RelatingReference}, {"RelatedResourceObjects", RelatedResourceObjects}}); }
inline Value IfcExternalSpatialElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value LongName, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcExternalSpatialElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"LongName", LongName}, {"PredefinedType", PredefinedType}}); }
inline Value IfcExternalSpatialStructureElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value LongName) { return ifcapi::express::make_entity("IFC4X1", "IfcExternalSpatialStructureElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"LongName", LongName}}); }
inline Value IfcExternallyDefinedHatchStyle(Value Location, Value Identification, Value Name) { return ifcapi::express::make_entity("IFC4X1", "IfcExternallyDefinedHatchStyle", {{"Location", Location}, {"Identification", Identification}, {"Name", Name}}); }
inline Value IfcExternallyDefinedSurfaceStyle(Value Location, Value Identification, Value Name) { return ifcapi::express::make_entity("IFC4X1", "IfcExternallyDefinedSurfaceStyle", {{"Location", Location}, {"Identification", Identification}, {"Name", Name}}); }
inline Value IfcExternallyDefinedTextFont(Value Location, Value Identification, Value Name) { return ifcapi::express::make_entity("IFC4X1", "IfcExternallyDefinedTextFont", {{"Location", Location}, {"Identification", Identification}, {"Name", Name}}); }
inline Value IfcExtrudedAreaSolid(Value SweptArea, Value Position, Value ExtrudedDirection, Value Depth) { return ifcapi::express::make_entity("IFC4X1", "IfcExtrudedAreaSolid", {{"SweptArea", SweptArea}, {"Position", Position}, {"ExtrudedDirection", ExtrudedDirection}, {"Depth", Depth}}); }
inline Value IfcExtrudedAreaSolidTapered(Value SweptArea, Value Position, Value ExtrudedDirection, Value Depth, Value EndSweptArea) { return ifcapi::express::make_entity("IFC4X1", "IfcExtrudedAreaSolidTapered", {{"SweptArea", SweptArea}, {"Position", Position}, {"ExtrudedDirection", ExtrudedDirection}, {"Depth", Depth}, {"EndSweptArea", EndSweptArea}}); }
inline Value IfcFace(Value Bounds) { return ifcapi::express::make_entity("IFC4X1", "IfcFace", {{"Bounds", Bounds}}); }
inline Value IfcFaceBasedSurfaceModel(Value FbsmFaces) { return ifcapi::express::make_entity("IFC4X1", "IfcFaceBasedSurfaceModel", {{"FbsmFaces", FbsmFaces}}); }
inline Value IfcFaceBound(Value Bound, Value Orientation) { return ifcapi::express::make_entity("IFC4X1", "IfcFaceBound", {{"Bound", Bound}, {"Orientation", Orientation}}); }
inline Value IfcFaceOuterBound(Value Bound, Value Orientation) { return ifcapi::express::make_entity("IFC4X1", "IfcFaceOuterBound", {{"Bound", Bound}, {"Orientation", Orientation}}); }
inline Value IfcFaceSurface(Value Bounds, Value FaceSurface, Value SameSense) { return ifcapi::express::make_entity("IFC4X1", "IfcFaceSurface", {{"Bounds", Bounds}, {"FaceSurface", FaceSurface}, {"SameSense", SameSense}}); }
inline Value IfcFacetedBrep(Value Outer) { return ifcapi::express::make_entity("IFC4X1", "IfcFacetedBrep", {{"Outer", Outer}}); }
inline Value IfcFacetedBrepWithVoids(Value Outer, Value Voids) { return ifcapi::express::make_entity("IFC4X1", "IfcFacetedBrepWithVoids", {{"Outer", Outer}, {"Voids", Voids}}); }
inline Value IfcFailureConnectionCondition(Value Name, Value TensionFailureX, Value TensionFailureY, Value TensionFailureZ, Value CompressionFailureX, Value CompressionFailureY, Value CompressionFailureZ) { return ifcapi::express::make_entity("IFC4X1", "IfcFailureConnectionCondition", {{"Name", Name}, {"TensionFailureX", TensionFailureX}, {"TensionFailureY", TensionFailureY}, {"TensionFailureZ", TensionFailureZ}, {"CompressionFailureX", CompressionFailureX}, {"CompressionFailureY", CompressionFailureY}, {"CompressionFailureZ", CompressionFailureZ}}); }
inline Value IfcFan(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcFan", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcFanType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcFanType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcFastener(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcFastener", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcFastenerType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcFastenerType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcFeatureElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC4X1", "IfcFeatureElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcFeatureElementAddition(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC4X1", "IfcFeatureElementAddition", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcFeatureElementSubtraction(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC4X1", "IfcFeatureElementSubtraction", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcFillAreaStyle(Value Name, Value FillStyles, Value ModelorDraughting) { return ifcapi::express::make_entity("IFC4X1", "IfcFillAreaStyle", {{"Name", Name}, {"FillStyles", FillStyles}, {"ModelorDraughting", ModelorDraughting}}); }
inline Value IfcFillAreaStyleHatching(Value HatchLineAppearance, Value StartOfNextHatchLine, Value PointOfReferenceHatchLine, Value PatternStart, Value HatchLineAngle) { return ifcapi::express::make_entity("IFC4X1", "IfcFillAreaStyleHatching", {{"HatchLineAppearance", HatchLineAppearance}, {"StartOfNextHatchLine", StartOfNextHatchLine}, {"PointOfReferenceHatchLine", PointOfReferenceHatchLine}, {"PatternStart", PatternStart}, {"HatchLineAngle", HatchLineAngle}}); }
inline Value IfcFillAreaStyleTiles(Value TilingPattern, Value Tiles, Value TilingScale) { return ifcapi::express::make_entity("IFC4X1", "IfcFillAreaStyleTiles", {{"TilingPattern", TilingPattern}, {"Tiles", Tiles}, {"TilingScale", TilingScale}}); }
inline Value IfcFilter(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcFilter", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcFilterType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcFilterType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcFireSuppressionTerminal(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcFireSuppressionTerminal", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcFireSuppressionTerminalType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcFireSuppressionTerminalType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcFixedReferenceSweptAreaSolid(Value SweptArea, Value Position, Value Directrix, Value StartParam, Value EndParam, Value FixedReference) { return ifcapi::express::make_entity("IFC4X1", "IfcFixedReferenceSweptAreaSolid", {{"SweptArea", SweptArea}, {"Position", Position}, {"Directrix", Directrix}, {"StartParam", StartParam}, {"EndParam", EndParam}, {"FixedReference", FixedReference}}); }
inline Value IfcFlowController(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC4X1", "IfcFlowController", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcFlowControllerType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC4X1", "IfcFlowControllerType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcFlowFitting(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC4X1", "IfcFlowFitting", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcFlowFittingType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC4X1", "IfcFlowFittingType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcFlowInstrument(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcFlowInstrument", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcFlowInstrumentType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcFlowInstrumentType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcFlowMeter(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcFlowMeter", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcFlowMeterType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcFlowMeterType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcFlowMovingDevice(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC4X1", "IfcFlowMovingDevice", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcFlowMovingDeviceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC4X1", "IfcFlowMovingDeviceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcFlowSegment(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC4X1", "IfcFlowSegment", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcFlowSegmentType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC4X1", "IfcFlowSegmentType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcFlowStorageDevice(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC4X1", "IfcFlowStorageDevice", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcFlowStorageDeviceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC4X1", "IfcFlowStorageDeviceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcFlowTerminal(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC4X1", "IfcFlowTerminal", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcFlowTerminalType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC4X1", "IfcFlowTerminalType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcFlowTreatmentDevice(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC4X1", "IfcFlowTreatmentDevice", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcFlowTreatmentDeviceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC4X1", "IfcFlowTreatmentDeviceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcFooting(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcFooting", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcFootingType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcFootingType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcFurnishingElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC4X1", "IfcFurnishingElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcFurnishingElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC4X1", "IfcFurnishingElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcFurniture(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcFurniture", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcFurnitureType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value AssemblyPlace, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcFurnitureType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"AssemblyPlace", AssemblyPlace}, {"PredefinedType", PredefinedType}}); }
inline Value IfcGeographicElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcGeographicElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcGeographicElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcGeographicElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcGeometricCurveSet(Value Elements) { return ifcapi::express::make_entity("IFC4X1", "IfcGeometricCurveSet", {{"Elements", Elements}}); }
inline Value IfcGeometricRepresentationContext(Value ContextIdentifier, Value ContextType, Value CoordinateSpaceDimension, Value Precision, Value WorldCoordinateSystem, Value TrueNorth) { return ifcapi::express::make_entity("IFC4X1", "IfcGeometricRepresentationContext", {{"ContextIdentifier", ContextIdentifier}, {"ContextType", ContextType}, {"CoordinateSpaceDimension", CoordinateSpaceDimension}, {"Precision", Precision}, {"WorldCoordinateSystem", WorldCoordinateSystem}, {"TrueNorth", TrueNorth}}); }
inline Value IfcGeometricRepresentationItem() { return ifcapi::express::make_entity("IFC4X1", "IfcGeometricRepresentationItem", {}); }
inline Value IfcGeometricRepresentationSubContext(Value ContextIdentifier, Value ContextType, Value CoordinateSpaceDimension, Value Precision, Value WorldCoordinateSystem, Value TrueNorth, Value ParentContext, Value TargetScale, Value TargetView, Value UserDefinedTargetView) { return ifcapi::express::make_entity("IFC4X1", "IfcGeometricRepresentationSubContext", {{"ContextIdentifier", ContextIdentifier}, {"ContextType", ContextType}, {"CoordinateSpaceDimension", CoordinateSpaceDimension}, {"Precision", Precision}, {"WorldCoordinateSystem", WorldCoordinateSystem}, {"TrueNorth", TrueNorth}, {"ParentContext", ParentContext}, {"TargetScale", TargetScale}, {"TargetView", TargetView}, {"UserDefinedTargetView", UserDefinedTargetView}}); }
inline Value IfcGeometricSet(Value Elements) { return ifcapi::express::make_entity("IFC4X1", "IfcGeometricSet", {{"Elements", Elements}}); }
inline Value IfcGrid(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value UAxes, Value VAxes, Value WAxes, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcGrid", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"UAxes", UAxes}, {"VAxes", VAxes}, {"WAxes", WAxes}, {"PredefinedType", PredefinedType}}); }
inline Value IfcGridAxis(Value AxisTag, Value AxisCurve, Value SameSense) { return ifcapi::express::make_entity("IFC4X1", "IfcGridAxis", {{"AxisTag", AxisTag}, {"AxisCurve", AxisCurve}, {"SameSense", SameSense}}); }
inline Value IfcGridPlacement(Value PlacementLocation, Value PlacementRefDirection) { return ifcapi::express::make_entity("IFC4X1", "IfcGridPlacement", {{"PlacementLocation", PlacementLocation}, {"PlacementRefDirection", PlacementRefDirection}}); }
inline Value IfcGroup(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType) { return ifcapi::express::make_entity("IFC4X1", "IfcGroup", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}}); }
inline Value IfcHalfSpaceSolid(Value BaseSurface, Value AgreementFlag) { return ifcapi::express::make_entity("IFC4X1", "IfcHalfSpaceSolid", {{"BaseSurface", BaseSurface}, {"AgreementFlag", AgreementFlag}}); }
inline Value IfcHeatExchanger(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcHeatExchanger", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcHeatExchangerType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcHeatExchangerType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcHumidifier(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcHumidifier", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcHumidifierType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcHumidifierType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcIShapeProfileDef(Value ProfileType, Value ProfileName, Value Position, Value OverallWidth, Value OverallDepth, Value WebThickness, Value FlangeThickness, Value FilletRadius, Value FlangeEdgeRadius, Value FlangeSlope) { return ifcapi::express::make_entity("IFC4X1", "IfcIShapeProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"OverallWidth", OverallWidth}, {"OverallDepth", OverallDepth}, {"WebThickness", WebThickness}, {"FlangeThickness", FlangeThickness}, {"FilletRadius", FilletRadius}, {"FlangeEdgeRadius", FlangeEdgeRadius}, {"FlangeSlope", FlangeSlope}}); }
inline Value IfcImageTexture(Value RepeatS, Value RepeatT, Value Mode, Value TextureTransform, Value Parameter, Value URLReference) { return ifcapi::express::make_entity("IFC4X1", "IfcImageTexture", {{"RepeatS", RepeatS}, {"RepeatT", RepeatT}, {"Mode", Mode}, {"TextureTransform", TextureTransform}, {"Parameter", Parameter}, {"URLReference", URLReference}}); }
inline Value IfcIndexedColourMap(Value MappedTo, Value Opacity, Value Colours, Value ColourIndex) { return ifcapi::express::make_entity("IFC4X1", "IfcIndexedColourMap", {{"MappedTo", MappedTo}, {"Opacity", Opacity}, {"Colours", Colours}, {"ColourIndex", ColourIndex}}); }
inline Value IfcIndexedPolyCurve(Value Points, Value Segments, Value SelfIntersect) { return ifcapi::express::make_entity("IFC4X1", "IfcIndexedPolyCurve", {{"Points", Points}, {"Segments", Segments}, {"SelfIntersect", SelfIntersect}}); }
inline Value IfcIndexedPolygonalFace(Value CoordIndex) { return ifcapi::express::make_entity("IFC4X1", "IfcIndexedPolygonalFace", {{"CoordIndex", CoordIndex}}); }
inline Value IfcIndexedPolygonalFaceWithVoids(Value CoordIndex, Value InnerCoordIndices) { return ifcapi::express::make_entity("IFC4X1", "IfcIndexedPolygonalFaceWithVoids", {{"CoordIndex", CoordIndex}, {"InnerCoordIndices", InnerCoordIndices}}); }
inline Value IfcIndexedTextureMap(Value Maps, Value MappedTo, Value TexCoords) { return ifcapi::express::make_entity("IFC4X1", "IfcIndexedTextureMap", {{"Maps", Maps}, {"MappedTo", MappedTo}, {"TexCoords", TexCoords}}); }
inline Value IfcIndexedTriangleTextureMap(Value Maps, Value MappedTo, Value TexCoords, Value TexCoordIndex) { return ifcapi::express::make_entity("IFC4X1", "IfcIndexedTriangleTextureMap", {{"Maps", Maps}, {"MappedTo", MappedTo}, {"TexCoords", TexCoords}, {"TexCoordIndex", TexCoordIndex}}); }
inline Value IfcInterceptor(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcInterceptor", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcInterceptorType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcInterceptorType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcIntersectionCurve(Value Curve3D, Value AssociatedGeometry, Value MasterRepresentation) { return ifcapi::express::make_entity("IFC4X1", "IfcIntersectionCurve", {{"Curve3D", Curve3D}, {"AssociatedGeometry", AssociatedGeometry}, {"MasterRepresentation", MasterRepresentation}}); }
inline Value IfcInventory(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value PredefinedType, Value Jurisdiction, Value ResponsiblePersons, Value LastUpdateDate, Value CurrentValue, Value OriginalValue) { return ifcapi::express::make_entity("IFC4X1", "IfcInventory", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"PredefinedType", PredefinedType}, {"Jurisdiction", Jurisdiction}, {"ResponsiblePersons", ResponsiblePersons}, {"LastUpdateDate", LastUpdateDate}, {"CurrentValue", CurrentValue}, {"OriginalValue", OriginalValue}}); }
inline Value IfcIrregularTimeSeries(Value Name, Value Description, Value StartTime, Value EndTime, Value TimeSeriesDataType, Value DataOrigin, Value UserDefinedDataOrigin, Value Unit, Value Values) { return ifcapi::express::make_entity("IFC4X1", "IfcIrregularTimeSeries", {{"Name", Name}, {"Description", Description}, {"StartTime", StartTime}, {"EndTime", EndTime}, {"TimeSeriesDataType", TimeSeriesDataType}, {"DataOrigin", DataOrigin}, {"UserDefinedDataOrigin", UserDefinedDataOrigin}, {"Unit", Unit}, {"Values", Values}}); }
inline Value IfcIrregularTimeSeriesValue(Value TimeStamp, Value ListValues) { return ifcapi::express::make_entity("IFC4X1", "IfcIrregularTimeSeriesValue", {{"TimeStamp", TimeStamp}, {"ListValues", ListValues}}); }
inline Value IfcJunctionBox(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcJunctionBox", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcJunctionBoxType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcJunctionBoxType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcLShapeProfileDef(Value ProfileType, Value ProfileName, Value Position, Value Depth, Value Width, Value Thickness, Value FilletRadius, Value EdgeRadius, Value LegSlope) { return ifcapi::express::make_entity("IFC4X1", "IfcLShapeProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"Depth", Depth}, {"Width", Width}, {"Thickness", Thickness}, {"FilletRadius", FilletRadius}, {"EdgeRadius", EdgeRadius}, {"LegSlope", LegSlope}}); }
inline Value IfcLaborResource(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identification, Value LongDescription, Value Usage, Value BaseCosts, Value BaseQuantity, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcLaborResource", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identification", Identification}, {"LongDescription", LongDescription}, {"Usage", Usage}, {"BaseCosts", BaseCosts}, {"BaseQuantity", BaseQuantity}, {"PredefinedType", PredefinedType}}); }
inline Value IfcLaborResourceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value Identification, Value LongDescription, Value ResourceType, Value BaseCosts, Value BaseQuantity, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcLaborResourceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"Identification", Identification}, {"LongDescription", LongDescription}, {"ResourceType", ResourceType}, {"BaseCosts", BaseCosts}, {"BaseQuantity", BaseQuantity}, {"PredefinedType", PredefinedType}}); }
inline Value IfcLagTime(Value Name, Value DataOrigin, Value UserDefinedDataOrigin, Value LagValue, Value DurationType) { return ifcapi::express::make_entity("IFC4X1", "IfcLagTime", {{"Name", Name}, {"DataOrigin", DataOrigin}, {"UserDefinedDataOrigin", UserDefinedDataOrigin}, {"LagValue", LagValue}, {"DurationType", DurationType}}); }
inline Value IfcLamp(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcLamp", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcLampType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcLampType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcLibraryInformation(Value Name, Value Version, Value Publisher, Value VersionDate, Value Location, Value Description) { return ifcapi::express::make_entity("IFC4X1", "IfcLibraryInformation", {{"Name", Name}, {"Version", Version}, {"Publisher", Publisher}, {"VersionDate", VersionDate}, {"Location", Location}, {"Description", Description}}); }
inline Value IfcLibraryReference(Value Location, Value Identification, Value Name, Value Description, Value Language, Value ReferencedLibrary) { return ifcapi::express::make_entity("IFC4X1", "IfcLibraryReference", {{"Location", Location}, {"Identification", Identification}, {"Name", Name}, {"Description", Description}, {"Language", Language}, {"ReferencedLibrary", ReferencedLibrary}}); }
inline Value IfcLightDistributionData(Value MainPlaneAngle, Value SecondaryPlaneAngle, Value LuminousIntensity) { return ifcapi::express::make_entity("IFC4X1", "IfcLightDistributionData", {{"MainPlaneAngle", MainPlaneAngle}, {"SecondaryPlaneAngle", SecondaryPlaneAngle}, {"LuminousIntensity", LuminousIntensity}}); }
inline Value IfcLightFixture(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcLightFixture", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcLightFixtureType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcLightFixtureType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcLightIntensityDistribution(Value LightDistributionCurve, Value DistributionData) { return ifcapi::express::make_entity("IFC4X1", "IfcLightIntensityDistribution", {{"LightDistributionCurve", LightDistributionCurve}, {"DistributionData", DistributionData}}); }
inline Value IfcLightSource(Value Name, Value LightColour, Value AmbientIntensity, Value Intensity) { return ifcapi::express::make_entity("IFC4X1", "IfcLightSource", {{"Name", Name}, {"LightColour", LightColour}, {"AmbientIntensity", AmbientIntensity}, {"Intensity", Intensity}}); }
inline Value IfcLightSourceAmbient(Value Name, Value LightColour, Value AmbientIntensity, Value Intensity) { return ifcapi::express::make_entity("IFC4X1", "IfcLightSourceAmbient", {{"Name", Name}, {"LightColour", LightColour}, {"AmbientIntensity", AmbientIntensity}, {"Intensity", Intensity}}); }
inline Value IfcLightSourceDirectional(Value Name, Value LightColour, Value AmbientIntensity, Value Intensity, Value Orientation) { return ifcapi::express::make_entity("IFC4X1", "IfcLightSourceDirectional", {{"Name", Name}, {"LightColour", LightColour}, {"AmbientIntensity", AmbientIntensity}, {"Intensity", Intensity}, {"Orientation", Orientation}}); }
inline Value IfcLightSourceGoniometric(Value Name, Value LightColour, Value AmbientIntensity, Value Intensity, Value Position, Value ColourAppearance, Value ColourTemperature, Value LuminousFlux, Value LightEmissionSource, Value LightDistributionDataSource) { return ifcapi::express::make_entity("IFC4X1", "IfcLightSourceGoniometric", {{"Name", Name}, {"LightColour", LightColour}, {"AmbientIntensity", AmbientIntensity}, {"Intensity", Intensity}, {"Position", Position}, {"ColourAppearance", ColourAppearance}, {"ColourTemperature", ColourTemperature}, {"LuminousFlux", LuminousFlux}, {"LightEmissionSource", LightEmissionSource}, {"LightDistributionDataSource", LightDistributionDataSource}}); }
inline Value IfcLightSourcePositional(Value Name, Value LightColour, Value AmbientIntensity, Value Intensity, Value Position, Value Radius, Value ConstantAttenuation, Value DistanceAttenuation, Value QuadricAttenuation) { return ifcapi::express::make_entity("IFC4X1", "IfcLightSourcePositional", {{"Name", Name}, {"LightColour", LightColour}, {"AmbientIntensity", AmbientIntensity}, {"Intensity", Intensity}, {"Position", Position}, {"Radius", Radius}, {"ConstantAttenuation", ConstantAttenuation}, {"DistanceAttenuation", DistanceAttenuation}, {"QuadricAttenuation", QuadricAttenuation}}); }
inline Value IfcLightSourceSpot(Value Name, Value LightColour, Value AmbientIntensity, Value Intensity, Value Position, Value Radius, Value ConstantAttenuation, Value DistanceAttenuation, Value QuadricAttenuation, Value Orientation, Value ConcentrationExponent, Value SpreadAngle, Value BeamWidthAngle) { return ifcapi::express::make_entity("IFC4X1", "IfcLightSourceSpot", {{"Name", Name}, {"LightColour", LightColour}, {"AmbientIntensity", AmbientIntensity}, {"Intensity", Intensity}, {"Position", Position}, {"Radius", Radius}, {"ConstantAttenuation", ConstantAttenuation}, {"DistanceAttenuation", DistanceAttenuation}, {"QuadricAttenuation", QuadricAttenuation}, {"Orientation", Orientation}, {"ConcentrationExponent", ConcentrationExponent}, {"SpreadAngle", SpreadAngle}, {"BeamWidthAngle", BeamWidthAngle}}); }
inline Value IfcLine(Value Pnt, Value Dir) { return ifcapi::express::make_entity("IFC4X1", "IfcLine", {{"Pnt", Pnt}, {"Dir", Dir}}); }
inline Value IfcLineSegment2D(Value StartPoint, Value StartDirection, Value SegmentLength) { return ifcapi::express::make_entity("IFC4X1", "IfcLineSegment2D", {{"StartPoint", StartPoint}, {"StartDirection", StartDirection}, {"SegmentLength", SegmentLength}}); }
inline Value IfcLinearPlacement(Value PlacementRelTo, Value Distance, Value Orientation, Value CartesianPosition) { return ifcapi::express::make_entity("IFC4X1", "IfcLinearPlacement", {{"PlacementRelTo", PlacementRelTo}, {"Distance", Distance}, {"Orientation", Orientation}, {"CartesianPosition", CartesianPosition}}); }
inline Value IfcLinearPositioningElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Axis) { return ifcapi::express::make_entity("IFC4X1", "IfcLinearPositioningElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Axis", Axis}}); }
inline Value IfcLocalPlacement(Value PlacementRelTo, Value RelativePlacement) { return ifcapi::express::make_entity("IFC4X1", "IfcLocalPlacement", {{"PlacementRelTo", PlacementRelTo}, {"RelativePlacement", RelativePlacement}}); }
inline Value IfcLoop() { return ifcapi::express::make_entity("IFC4X1", "IfcLoop", {}); }
inline Value IfcManifoldSolidBrep(Value Outer) { return ifcapi::express::make_entity("IFC4X1", "IfcManifoldSolidBrep", {{"Outer", Outer}}); }
inline Value IfcMapConversion(Value SourceCRS, Value TargetCRS, Value Eastings, Value Northings, Value OrthogonalHeight, Value XAxisAbscissa, Value XAxisOrdinate, Value Scale) { return ifcapi::express::make_entity("IFC4X1", "IfcMapConversion", {{"SourceCRS", SourceCRS}, {"TargetCRS", TargetCRS}, {"Eastings", Eastings}, {"Northings", Northings}, {"OrthogonalHeight", OrthogonalHeight}, {"XAxisAbscissa", XAxisAbscissa}, {"XAxisOrdinate", XAxisOrdinate}, {"Scale", Scale}}); }
inline Value IfcMappedItem(Value MappingSource, Value MappingTarget) { return ifcapi::express::make_entity("IFC4X1", "IfcMappedItem", {{"MappingSource", MappingSource}, {"MappingTarget", MappingTarget}}); }
inline Value IfcMaterial(Value Name, Value Description, Value Category) { return ifcapi::express::make_entity("IFC4X1", "IfcMaterial", {{"Name", Name}, {"Description", Description}, {"Category", Category}}); }
inline Value IfcMaterialClassificationRelationship(Value MaterialClassifications, Value ClassifiedMaterial) { return ifcapi::express::make_entity("IFC4X1", "IfcMaterialClassificationRelationship", {{"MaterialClassifications", MaterialClassifications}, {"ClassifiedMaterial", ClassifiedMaterial}}); }
inline Value IfcMaterialConstituent(Value Name, Value Description, Value Material, Value Fraction, Value Category) { return ifcapi::express::make_entity("IFC4X1", "IfcMaterialConstituent", {{"Name", Name}, {"Description", Description}, {"Material", Material}, {"Fraction", Fraction}, {"Category", Category}}); }
inline Value IfcMaterialConstituentSet(Value Name, Value Description, Value MaterialConstituents) { return ifcapi::express::make_entity("IFC4X1", "IfcMaterialConstituentSet", {{"Name", Name}, {"Description", Description}, {"MaterialConstituents", MaterialConstituents}}); }
inline Value IfcMaterialDefinition() { return ifcapi::express::make_entity("IFC4X1", "IfcMaterialDefinition", {}); }
inline Value IfcMaterialDefinitionRepresentation(Value Name, Value Description, Value Representations, Value RepresentedMaterial) { return ifcapi::express::make_entity("IFC4X1", "IfcMaterialDefinitionRepresentation", {{"Name", Name}, {"Description", Description}, {"Representations", Representations}, {"RepresentedMaterial", RepresentedMaterial}}); }
inline Value IfcMaterialLayer(Value Material, Value LayerThickness, Value IsVentilated, Value Name, Value Description, Value Category, Value Priority) { return ifcapi::express::make_entity("IFC4X1", "IfcMaterialLayer", {{"Material", Material}, {"LayerThickness", LayerThickness}, {"IsVentilated", IsVentilated}, {"Name", Name}, {"Description", Description}, {"Category", Category}, {"Priority", Priority}}); }
inline Value IfcMaterialLayerSet(Value MaterialLayers, Value LayerSetName, Value Description) { return ifcapi::express::make_entity("IFC4X1", "IfcMaterialLayerSet", {{"MaterialLayers", MaterialLayers}, {"LayerSetName", LayerSetName}, {"Description", Description}}); }
inline Value IfcMaterialLayerSetUsage(Value ForLayerSet, Value LayerSetDirection, Value DirectionSense, Value OffsetFromReferenceLine, Value ReferenceExtent) { return ifcapi::express::make_entity("IFC4X1", "IfcMaterialLayerSetUsage", {{"ForLayerSet", ForLayerSet}, {"LayerSetDirection", LayerSetDirection}, {"DirectionSense", DirectionSense}, {"OffsetFromReferenceLine", OffsetFromReferenceLine}, {"ReferenceExtent", ReferenceExtent}}); }
inline Value IfcMaterialLayerWithOffsets(Value Material, Value LayerThickness, Value IsVentilated, Value Name, Value Description, Value Category, Value Priority, Value OffsetDirection, Value OffsetValues) { return ifcapi::express::make_entity("IFC4X1", "IfcMaterialLayerWithOffsets", {{"Material", Material}, {"LayerThickness", LayerThickness}, {"IsVentilated", IsVentilated}, {"Name", Name}, {"Description", Description}, {"Category", Category}, {"Priority", Priority}, {"OffsetDirection", OffsetDirection}, {"OffsetValues", OffsetValues}}); }
inline Value IfcMaterialList(Value Materials) { return ifcapi::express::make_entity("IFC4X1", "IfcMaterialList", {{"Materials", Materials}}); }
inline Value IfcMaterialProfile(Value Name, Value Description, Value Material, Value Profile, Value Priority, Value Category) { return ifcapi::express::make_entity("IFC4X1", "IfcMaterialProfile", {{"Name", Name}, {"Description", Description}, {"Material", Material}, {"Profile", Profile}, {"Priority", Priority}, {"Category", Category}}); }
inline Value IfcMaterialProfileSet(Value Name, Value Description, Value MaterialProfiles, Value CompositeProfile) { return ifcapi::express::make_entity("IFC4X1", "IfcMaterialProfileSet", {{"Name", Name}, {"Description", Description}, {"MaterialProfiles", MaterialProfiles}, {"CompositeProfile", CompositeProfile}}); }
inline Value IfcMaterialProfileSetUsage(Value ForProfileSet, Value CardinalPoint, Value ReferenceExtent) { return ifcapi::express::make_entity("IFC4X1", "IfcMaterialProfileSetUsage", {{"ForProfileSet", ForProfileSet}, {"CardinalPoint", CardinalPoint}, {"ReferenceExtent", ReferenceExtent}}); }
inline Value IfcMaterialProfileSetUsageTapering(Value ForProfileSet, Value CardinalPoint, Value ReferenceExtent, Value ForProfileEndSet, Value CardinalEndPoint) { return ifcapi::express::make_entity("IFC4X1", "IfcMaterialProfileSetUsageTapering", {{"ForProfileSet", ForProfileSet}, {"CardinalPoint", CardinalPoint}, {"ReferenceExtent", ReferenceExtent}, {"ForProfileEndSet", ForProfileEndSet}, {"CardinalEndPoint", CardinalEndPoint}}); }
inline Value IfcMaterialProfileWithOffsets(Value Name, Value Description, Value Material, Value Profile, Value Priority, Value Category, Value OffsetValues) { return ifcapi::express::make_entity("IFC4X1", "IfcMaterialProfileWithOffsets", {{"Name", Name}, {"Description", Description}, {"Material", Material}, {"Profile", Profile}, {"Priority", Priority}, {"Category", Category}, {"OffsetValues", OffsetValues}}); }
inline Value IfcMaterialProperties(Value Name, Value Description, Value Properties, Value Material) { return ifcapi::express::make_entity("IFC4X1", "IfcMaterialProperties", {{"Name", Name}, {"Description", Description}, {"Properties", Properties}, {"Material", Material}}); }
inline Value IfcMaterialRelationship(Value Name, Value Description, Value RelatingMaterial, Value RelatedMaterials, Value Expression) { return ifcapi::express::make_entity("IFC4X1", "IfcMaterialRelationship", {{"Name", Name}, {"Description", Description}, {"RelatingMaterial", RelatingMaterial}, {"RelatedMaterials", RelatedMaterials}, {"Expression", Expression}}); }
inline Value IfcMaterialUsageDefinition() { return ifcapi::express::make_entity("IFC4X1", "IfcMaterialUsageDefinition", {}); }
inline Value IfcMeasureWithUnit(Value ValueComponent, Value UnitComponent) { return ifcapi::express::make_entity("IFC4X1", "IfcMeasureWithUnit", {{"ValueComponent", ValueComponent}, {"UnitComponent", UnitComponent}}); }
inline Value IfcMechanicalFastener(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value NominalDiameter, Value NominalLength, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcMechanicalFastener", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"NominalDiameter", NominalDiameter}, {"NominalLength", NominalLength}, {"PredefinedType", PredefinedType}}); }
inline Value IfcMechanicalFastenerType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType, Value NominalDiameter, Value NominalLength) { return ifcapi::express::make_entity("IFC4X1", "IfcMechanicalFastenerType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}, {"NominalDiameter", NominalDiameter}, {"NominalLength", NominalLength}}); }
inline Value IfcMedicalDevice(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcMedicalDevice", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcMedicalDeviceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcMedicalDeviceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcMember(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcMember", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcMemberStandardCase(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcMemberStandardCase", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcMemberType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcMemberType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcMetric(Value Name, Value Description, Value ConstraintGrade, Value ConstraintSource, Value CreatingActor, Value CreationTime, Value UserDefinedGrade, Value Benchmark, Value ValueSource, Value DataValue, Value ReferencePath) { return ifcapi::express::make_entity("IFC4X1", "IfcMetric", {{"Name", Name}, {"Description", Description}, {"ConstraintGrade", ConstraintGrade}, {"ConstraintSource", ConstraintSource}, {"CreatingActor", CreatingActor}, {"CreationTime", CreationTime}, {"UserDefinedGrade", UserDefinedGrade}, {"Benchmark", Benchmark}, {"ValueSource", ValueSource}, {"DataValue", DataValue}, {"ReferencePath", ReferencePath}}); }
inline Value IfcMirroredProfileDef(Value ProfileType, Value ProfileName, Value ParentProfile, Value Operator_, Value Label) { return ifcapi::express::make_entity("IFC4X1", "IfcMirroredProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"ParentProfile", ParentProfile}, {"Operator", Operator_}, {"Label", Label}}); }
inline Value IfcMonetaryUnit(Value Currency) { return ifcapi::express::make_entity("IFC4X1", "IfcMonetaryUnit", {{"Currency", Currency}}); }
inline Value IfcMotorConnection(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcMotorConnection", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcMotorConnectionType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcMotorConnectionType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcNamedUnit(Value Dimensions, Value UnitType) { return ifcapi::express::make_entity("IFC4X1", "IfcNamedUnit", {{"Dimensions", Dimensions}, {"UnitType", UnitType}}); }
inline Value IfcObject(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType) { return ifcapi::express::make_entity("IFC4X1", "IfcObject", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}}); }
inline Value IfcObjectDefinition(Value GlobalId, Value OwnerHistory, Value Name, Value Description) { return ifcapi::express::make_entity("IFC4X1", "IfcObjectDefinition", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}}); }
inline Value IfcObjectPlacement() { return ifcapi::express::make_entity("IFC4X1", "IfcObjectPlacement", {}); }
inline Value IfcObjective(Value Name, Value Description, Value ConstraintGrade, Value ConstraintSource, Value CreatingActor, Value CreationTime, Value UserDefinedGrade, Value BenchmarkValues, Value LogicalAggregator, Value ObjectiveQualifier, Value UserDefinedQualifier) { return ifcapi::express::make_entity("IFC4X1", "IfcObjective", {{"Name", Name}, {"Description", Description}, {"ConstraintGrade", ConstraintGrade}, {"ConstraintSource", ConstraintSource}, {"CreatingActor", CreatingActor}, {"CreationTime", CreationTime}, {"UserDefinedGrade", UserDefinedGrade}, {"BenchmarkValues", BenchmarkValues}, {"LogicalAggregator", LogicalAggregator}, {"ObjectiveQualifier", ObjectiveQualifier}, {"UserDefinedQualifier", UserDefinedQualifier}}); }
inline Value IfcOccupant(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value TheActor, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcOccupant", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"TheActor", TheActor}, {"PredefinedType", PredefinedType}}); }
inline Value IfcOffsetCurve(Value BasisCurve) { return ifcapi::express::make_entity("IFC4X1", "IfcOffsetCurve", {{"BasisCurve", BasisCurve}}); }
inline Value IfcOffsetCurve2D(Value BasisCurve, Value Distance, Value SelfIntersect) { return ifcapi::express::make_entity("IFC4X1", "IfcOffsetCurve2D", {{"BasisCurve", BasisCurve}, {"Distance", Distance}, {"SelfIntersect", SelfIntersect}}); }
inline Value IfcOffsetCurve3D(Value BasisCurve, Value Distance, Value SelfIntersect, Value RefDirection) { return ifcapi::express::make_entity("IFC4X1", "IfcOffsetCurve3D", {{"BasisCurve", BasisCurve}, {"Distance", Distance}, {"SelfIntersect", SelfIntersect}, {"RefDirection", RefDirection}}); }
inline Value IfcOffsetCurveByDistances(Value BasisCurve, Value OffsetValues, Value Tag) { return ifcapi::express::make_entity("IFC4X1", "IfcOffsetCurveByDistances", {{"BasisCurve", BasisCurve}, {"OffsetValues", OffsetValues}, {"Tag", Tag}}); }
inline Value IfcOpenShell(Value CfsFaces) { return ifcapi::express::make_entity("IFC4X1", "IfcOpenShell", {{"CfsFaces", CfsFaces}}); }
inline Value IfcOpeningElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcOpeningElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcOpeningStandardCase(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcOpeningStandardCase", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcOrganization(Value Identification, Value Name, Value Description, Value Roles, Value Addresses) { return ifcapi::express::make_entity("IFC4X1", "IfcOrganization", {{"Identification", Identification}, {"Name", Name}, {"Description", Description}, {"Roles", Roles}, {"Addresses", Addresses}}); }
inline Value IfcOrganizationRelationship(Value Name, Value Description, Value RelatingOrganization, Value RelatedOrganizations) { return ifcapi::express::make_entity("IFC4X1", "IfcOrganizationRelationship", {{"Name", Name}, {"Description", Description}, {"RelatingOrganization", RelatingOrganization}, {"RelatedOrganizations", RelatedOrganizations}}); }
inline Value IfcOrientationExpression(Value LateralAxisDirection, Value VerticalAxisDirection) { return ifcapi::express::make_entity("IFC4X1", "IfcOrientationExpression", {{"LateralAxisDirection", LateralAxisDirection}, {"VerticalAxisDirection", VerticalAxisDirection}}); }
inline Value IfcOrientedEdge(Value EdgeStart, Value EdgeEnd, Value EdgeElement, Value Orientation) { return ifcapi::express::make_entity("IFC4X1", "IfcOrientedEdge", {{"EdgeStart", EdgeStart}, {"EdgeEnd", EdgeEnd}, {"EdgeElement", EdgeElement}, {"Orientation", Orientation}}); }
inline Value IfcOuterBoundaryCurve(Value Segments, Value SelfIntersect) { return ifcapi::express::make_entity("IFC4X1", "IfcOuterBoundaryCurve", {{"Segments", Segments}, {"SelfIntersect", SelfIntersect}}); }
inline Value IfcOutlet(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcOutlet", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcOutletType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcOutletType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcOwnerHistory(Value OwningUser, Value OwningApplication, Value State, Value ChangeAction, Value LastModifiedDate, Value LastModifyingUser, Value LastModifyingApplication, Value CreationDate) { return ifcapi::express::make_entity("IFC4X1", "IfcOwnerHistory", {{"OwningUser", OwningUser}, {"OwningApplication", OwningApplication}, {"State", State}, {"ChangeAction", ChangeAction}, {"LastModifiedDate", LastModifiedDate}, {"LastModifyingUser", LastModifyingUser}, {"LastModifyingApplication", LastModifyingApplication}, {"CreationDate", CreationDate}}); }
inline Value IfcParameterizedProfileDef(Value ProfileType, Value ProfileName, Value Position) { return ifcapi::express::make_entity("IFC4X1", "IfcParameterizedProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}}); }
inline Value IfcPath(Value EdgeList) { return ifcapi::express::make_entity("IFC4X1", "IfcPath", {{"EdgeList", EdgeList}}); }
inline Value IfcPcurve(Value BasisSurface, Value ReferenceCurve) { return ifcapi::express::make_entity("IFC4X1", "IfcPcurve", {{"BasisSurface", BasisSurface}, {"ReferenceCurve", ReferenceCurve}}); }
inline Value IfcPerformanceHistory(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identification, Value LifeCyclePhase, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcPerformanceHistory", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identification", Identification}, {"LifeCyclePhase", LifeCyclePhase}, {"PredefinedType", PredefinedType}}); }
inline Value IfcPermeableCoveringProperties(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value OperationType, Value PanelPosition, Value FrameDepth, Value FrameThickness, Value ShapeAspectStyle) { return ifcapi::express::make_entity("IFC4X1", "IfcPermeableCoveringProperties", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"OperationType", OperationType}, {"PanelPosition", PanelPosition}, {"FrameDepth", FrameDepth}, {"FrameThickness", FrameThickness}, {"ShapeAspectStyle", ShapeAspectStyle}}); }
inline Value IfcPermit(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identification, Value PredefinedType, Value Status, Value LongDescription) { return ifcapi::express::make_entity("IFC4X1", "IfcPermit", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identification", Identification}, {"PredefinedType", PredefinedType}, {"Status", Status}, {"LongDescription", LongDescription}}); }
inline Value IfcPerson(Value Identification, Value FamilyName, Value GivenName, Value MiddleNames, Value PrefixTitles, Value SuffixTitles, Value Roles, Value Addresses) { return ifcapi::express::make_entity("IFC4X1", "IfcPerson", {{"Identification", Identification}, {"FamilyName", FamilyName}, {"GivenName", GivenName}, {"MiddleNames", MiddleNames}, {"PrefixTitles", PrefixTitles}, {"SuffixTitles", SuffixTitles}, {"Roles", Roles}, {"Addresses", Addresses}}); }
inline Value IfcPersonAndOrganization(Value ThePerson, Value TheOrganization, Value Roles) { return ifcapi::express::make_entity("IFC4X1", "IfcPersonAndOrganization", {{"ThePerson", ThePerson}, {"TheOrganization", TheOrganization}, {"Roles", Roles}}); }
inline Value IfcPhysicalComplexQuantity(Value Name, Value Description, Value HasQuantities, Value Discrimination, Value Quality, Value Usage) { return ifcapi::express::make_entity("IFC4X1", "IfcPhysicalComplexQuantity", {{"Name", Name}, {"Description", Description}, {"HasQuantities", HasQuantities}, {"Discrimination", Discrimination}, {"Quality", Quality}, {"Usage", Usage}}); }
inline Value IfcPhysicalQuantity(Value Name, Value Description) { return ifcapi::express::make_entity("IFC4X1", "IfcPhysicalQuantity", {{"Name", Name}, {"Description", Description}}); }
inline Value IfcPhysicalSimpleQuantity(Value Name, Value Description, Value Unit) { return ifcapi::express::make_entity("IFC4X1", "IfcPhysicalSimpleQuantity", {{"Name", Name}, {"Description", Description}, {"Unit", Unit}}); }
inline Value IfcPile(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType, Value ConstructionType) { return ifcapi::express::make_entity("IFC4X1", "IfcPile", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}, {"ConstructionType", ConstructionType}}); }
inline Value IfcPileType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcPileType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcPipeFitting(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcPipeFitting", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcPipeFittingType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcPipeFittingType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcPipeSegment(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcPipeSegment", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcPipeSegmentType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcPipeSegmentType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcPixelTexture(Value RepeatS, Value RepeatT, Value Mode, Value TextureTransform, Value Parameter, Value Width, Value Height, Value ColourComponents, Value Pixel) { return ifcapi::express::make_entity("IFC4X1", "IfcPixelTexture", {{"RepeatS", RepeatS}, {"RepeatT", RepeatT}, {"Mode", Mode}, {"TextureTransform", TextureTransform}, {"Parameter", Parameter}, {"Width", Width}, {"Height", Height}, {"ColourComponents", ColourComponents}, {"Pixel", Pixel}}); }
inline Value IfcPlacement(Value Location) { return ifcapi::express::make_entity("IFC4X1", "IfcPlacement", {{"Location", Location}}); }
inline Value IfcPlanarBox(Value SizeInX, Value SizeInY, Value Placement) { return ifcapi::express::make_entity("IFC4X1", "IfcPlanarBox", {{"SizeInX", SizeInX}, {"SizeInY", SizeInY}, {"Placement", Placement}}); }
inline Value IfcPlanarExtent(Value SizeInX, Value SizeInY) { return ifcapi::express::make_entity("IFC4X1", "IfcPlanarExtent", {{"SizeInX", SizeInX}, {"SizeInY", SizeInY}}); }
inline Value IfcPlane(Value Position) { return ifcapi::express::make_entity("IFC4X1", "IfcPlane", {{"Position", Position}}); }
inline Value IfcPlate(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcPlate", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcPlateStandardCase(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcPlateStandardCase", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcPlateType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcPlateType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcPoint() { return ifcapi::express::make_entity("IFC4X1", "IfcPoint", {}); }
inline Value IfcPointOnCurve(Value BasisCurve, Value PointParameter) { return ifcapi::express::make_entity("IFC4X1", "IfcPointOnCurve", {{"BasisCurve", BasisCurve}, {"PointParameter", PointParameter}}); }
inline Value IfcPointOnSurface(Value BasisSurface, Value PointParameterU, Value PointParameterV) { return ifcapi::express::make_entity("IFC4X1", "IfcPointOnSurface", {{"BasisSurface", BasisSurface}, {"PointParameterU", PointParameterU}, {"PointParameterV", PointParameterV}}); }
inline Value IfcPolyLoop(Value Polygon) { return ifcapi::express::make_entity("IFC4X1", "IfcPolyLoop", {{"Polygon", Polygon}}); }
inline Value IfcPolygonalBoundedHalfSpace(Value BaseSurface, Value AgreementFlag, Value Position, Value PolygonalBoundary) { return ifcapi::express::make_entity("IFC4X1", "IfcPolygonalBoundedHalfSpace", {{"BaseSurface", BaseSurface}, {"AgreementFlag", AgreementFlag}, {"Position", Position}, {"PolygonalBoundary", PolygonalBoundary}}); }
inline Value IfcPolygonalFaceSet(Value Coordinates, Value Closed, Value Faces, Value PnIndex) { return ifcapi::express::make_entity("IFC4X1", "IfcPolygonalFaceSet", {{"Coordinates", Coordinates}, {"Closed", Closed}, {"Faces", Faces}, {"PnIndex", PnIndex}}); }
inline Value IfcPolyline(Value Points) { return ifcapi::express::make_entity("IFC4X1", "IfcPolyline", {{"Points", Points}}); }
inline Value IfcPort(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation) { return ifcapi::express::make_entity("IFC4X1", "IfcPort", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}}); }
inline Value IfcPositioningElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation) { return ifcapi::express::make_entity("IFC4X1", "IfcPositioningElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}}); }
inline Value IfcPostalAddress(Value Purpose, Value Description, Value UserDefinedPurpose, Value InternalLocation, Value AddressLines, Value PostalBox, Value Town, Value Region, Value PostalCode, Value Country) { return ifcapi::express::make_entity("IFC4X1", "IfcPostalAddress", {{"Purpose", Purpose}, {"Description", Description}, {"UserDefinedPurpose", UserDefinedPurpose}, {"InternalLocation", InternalLocation}, {"AddressLines", AddressLines}, {"PostalBox", PostalBox}, {"Town", Town}, {"Region", Region}, {"PostalCode", PostalCode}, {"Country", Country}}); }
inline Value IfcPreDefinedColour(Value Name) { return ifcapi::express::make_entity("IFC4X1", "IfcPreDefinedColour", {{"Name", Name}}); }
inline Value IfcPreDefinedCurveFont(Value Name) { return ifcapi::express::make_entity("IFC4X1", "IfcPreDefinedCurveFont", {{"Name", Name}}); }
inline Value IfcPreDefinedItem(Value Name) { return ifcapi::express::make_entity("IFC4X1", "IfcPreDefinedItem", {{"Name", Name}}); }
inline Value IfcPreDefinedProperties() { return ifcapi::express::make_entity("IFC4X1", "IfcPreDefinedProperties", {}); }
inline Value IfcPreDefinedPropertySet(Value GlobalId, Value OwnerHistory, Value Name, Value Description) { return ifcapi::express::make_entity("IFC4X1", "IfcPreDefinedPropertySet", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}}); }
inline Value IfcPreDefinedTextFont(Value Name) { return ifcapi::express::make_entity("IFC4X1", "IfcPreDefinedTextFont", {{"Name", Name}}); }
inline Value IfcPresentationItem() { return ifcapi::express::make_entity("IFC4X1", "IfcPresentationItem", {}); }
inline Value IfcPresentationLayerAssignment(Value Name, Value Description, Value AssignedItems, Value Identifier) { return ifcapi::express::make_entity("IFC4X1", "IfcPresentationLayerAssignment", {{"Name", Name}, {"Description", Description}, {"AssignedItems", AssignedItems}, {"Identifier", Identifier}}); }
inline Value IfcPresentationLayerWithStyle(Value Name, Value Description, Value AssignedItems, Value Identifier, Value LayerOn, Value LayerFrozen, Value LayerBlocked, Value LayerStyles) { return ifcapi::express::make_entity("IFC4X1", "IfcPresentationLayerWithStyle", {{"Name", Name}, {"Description", Description}, {"AssignedItems", AssignedItems}, {"Identifier", Identifier}, {"LayerOn", LayerOn}, {"LayerFrozen", LayerFrozen}, {"LayerBlocked", LayerBlocked}, {"LayerStyles", LayerStyles}}); }
inline Value IfcPresentationStyle(Value Name) { return ifcapi::express::make_entity("IFC4X1", "IfcPresentationStyle", {{"Name", Name}}); }
inline Value IfcPresentationStyleAssignment(Value Styles) { return ifcapi::express::make_entity("IFC4X1", "IfcPresentationStyleAssignment", {{"Styles", Styles}}); }
inline Value IfcProcedure(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identification, Value LongDescription, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcProcedure", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identification", Identification}, {"LongDescription", LongDescription}, {"PredefinedType", PredefinedType}}); }
inline Value IfcProcedureType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value Identification, Value LongDescription, Value ProcessType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcProcedureType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"Identification", Identification}, {"LongDescription", LongDescription}, {"ProcessType", ProcessType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcProcess(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identification, Value LongDescription) { return ifcapi::express::make_entity("IFC4X1", "IfcProcess", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identification", Identification}, {"LongDescription", LongDescription}}); }
inline Value IfcProduct(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation) { return ifcapi::express::make_entity("IFC4X1", "IfcProduct", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}}); }
inline Value IfcProductDefinitionShape(Value Name, Value Description, Value Representations) { return ifcapi::express::make_entity("IFC4X1", "IfcProductDefinitionShape", {{"Name", Name}, {"Description", Description}, {"Representations", Representations}}); }
inline Value IfcProductRepresentation(Value Name, Value Description, Value Representations) { return ifcapi::express::make_entity("IFC4X1", "IfcProductRepresentation", {{"Name", Name}, {"Description", Description}, {"Representations", Representations}}); }
inline Value IfcProfileDef(Value ProfileType, Value ProfileName) { return ifcapi::express::make_entity("IFC4X1", "IfcProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}}); }
inline Value IfcProfileProperties(Value Name, Value Description, Value Properties, Value ProfileDefinition) { return ifcapi::express::make_entity("IFC4X1", "IfcProfileProperties", {{"Name", Name}, {"Description", Description}, {"Properties", Properties}, {"ProfileDefinition", ProfileDefinition}}); }
inline Value IfcProject(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value LongName, Value Phase, Value RepresentationContexts, Value UnitsInContext) { return ifcapi::express::make_entity("IFC4X1", "IfcProject", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"LongName", LongName}, {"Phase", Phase}, {"RepresentationContexts", RepresentationContexts}, {"UnitsInContext", UnitsInContext}}); }
inline Value IfcProjectLibrary(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value LongName, Value Phase, Value RepresentationContexts, Value UnitsInContext) { return ifcapi::express::make_entity("IFC4X1", "IfcProjectLibrary", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"LongName", LongName}, {"Phase", Phase}, {"RepresentationContexts", RepresentationContexts}, {"UnitsInContext", UnitsInContext}}); }
inline Value IfcProjectOrder(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identification, Value PredefinedType, Value Status, Value LongDescription) { return ifcapi::express::make_entity("IFC4X1", "IfcProjectOrder", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identification", Identification}, {"PredefinedType", PredefinedType}, {"Status", Status}, {"LongDescription", LongDescription}}); }
inline Value IfcProjectedCRS(Value Name, Value Description, Value GeodeticDatum, Value VerticalDatum, Value MapProjection, Value MapZone, Value MapUnit) { return ifcapi::express::make_entity("IFC4X1", "IfcProjectedCRS", {{"Name", Name}, {"Description", Description}, {"GeodeticDatum", GeodeticDatum}, {"VerticalDatum", VerticalDatum}, {"MapProjection", MapProjection}, {"MapZone", MapZone}, {"MapUnit", MapUnit}}); }
inline Value IfcProjectionElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcProjectionElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcProperty(Value Name, Value Description) { return ifcapi::express::make_entity("IFC4X1", "IfcProperty", {{"Name", Name}, {"Description", Description}}); }
inline Value IfcPropertyAbstraction() { return ifcapi::express::make_entity("IFC4X1", "IfcPropertyAbstraction", {}); }
inline Value IfcPropertyBoundedValue(Value Name, Value Description, Value UpperBoundValue, Value LowerBoundValue, Value Unit, Value SetPointValue) { return ifcapi::express::make_entity("IFC4X1", "IfcPropertyBoundedValue", {{"Name", Name}, {"Description", Description}, {"UpperBoundValue", UpperBoundValue}, {"LowerBoundValue", LowerBoundValue}, {"Unit", Unit}, {"SetPointValue", SetPointValue}}); }
inline Value IfcPropertyDefinition(Value GlobalId, Value OwnerHistory, Value Name, Value Description) { return ifcapi::express::make_entity("IFC4X1", "IfcPropertyDefinition", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}}); }
inline Value IfcPropertyDependencyRelationship(Value Name, Value Description, Value DependingProperty, Value DependantProperty, Value Expression) { return ifcapi::express::make_entity("IFC4X1", "IfcPropertyDependencyRelationship", {{"Name", Name}, {"Description", Description}, {"DependingProperty", DependingProperty}, {"DependantProperty", DependantProperty}, {"Expression", Expression}}); }
inline Value IfcPropertyEnumeratedValue(Value Name, Value Description, Value EnumerationValues, Value EnumerationReference) { return ifcapi::express::make_entity("IFC4X1", "IfcPropertyEnumeratedValue", {{"Name", Name}, {"Description", Description}, {"EnumerationValues", EnumerationValues}, {"EnumerationReference", EnumerationReference}}); }
inline Value IfcPropertyEnumeration(Value Name, Value EnumerationValues, Value Unit) { return ifcapi::express::make_entity("IFC4X1", "IfcPropertyEnumeration", {{"Name", Name}, {"EnumerationValues", EnumerationValues}, {"Unit", Unit}}); }
inline Value IfcPropertyListValue(Value Name, Value Description, Value ListValues, Value Unit) { return ifcapi::express::make_entity("IFC4X1", "IfcPropertyListValue", {{"Name", Name}, {"Description", Description}, {"ListValues", ListValues}, {"Unit", Unit}}); }
inline Value IfcPropertyReferenceValue(Value Name, Value Description, Value UsageName, Value PropertyReference) { return ifcapi::express::make_entity("IFC4X1", "IfcPropertyReferenceValue", {{"Name", Name}, {"Description", Description}, {"UsageName", UsageName}, {"PropertyReference", PropertyReference}}); }
inline Value IfcPropertySet(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value HasProperties) { return ifcapi::express::make_entity("IFC4X1", "IfcPropertySet", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"HasProperties", HasProperties}}); }
inline Value IfcPropertySetDefinition(Value GlobalId, Value OwnerHistory, Value Name, Value Description) { return ifcapi::express::make_entity("IFC4X1", "IfcPropertySetDefinition", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}}); }
inline Value IfcPropertySetTemplate(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value TemplateType, Value ApplicableEntity, Value HasPropertyTemplates) { return ifcapi::express::make_entity("IFC4X1", "IfcPropertySetTemplate", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"TemplateType", TemplateType}, {"ApplicableEntity", ApplicableEntity}, {"HasPropertyTemplates", HasPropertyTemplates}}); }
inline Value IfcPropertySingleValue(Value Name, Value Description, Value NominalValue, Value Unit) { return ifcapi::express::make_entity("IFC4X1", "IfcPropertySingleValue", {{"Name", Name}, {"Description", Description}, {"NominalValue", NominalValue}, {"Unit", Unit}}); }
inline Value IfcPropertyTableValue(Value Name, Value Description, Value DefiningValues, Value DefinedValues, Value Expression, Value DefiningUnit, Value DefinedUnit, Value CurveInterpolation) { return ifcapi::express::make_entity("IFC4X1", "IfcPropertyTableValue", {{"Name", Name}, {"Description", Description}, {"DefiningValues", DefiningValues}, {"DefinedValues", DefinedValues}, {"Expression", Expression}, {"DefiningUnit", DefiningUnit}, {"DefinedUnit", DefinedUnit}, {"CurveInterpolation", CurveInterpolation}}); }
inline Value IfcPropertyTemplate(Value GlobalId, Value OwnerHistory, Value Name, Value Description) { return ifcapi::express::make_entity("IFC4X1", "IfcPropertyTemplate", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}}); }
inline Value IfcPropertyTemplateDefinition(Value GlobalId, Value OwnerHistory, Value Name, Value Description) { return ifcapi::express::make_entity("IFC4X1", "IfcPropertyTemplateDefinition", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}}); }
inline Value IfcProtectiveDevice(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcProtectiveDevice", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcProtectiveDeviceTrippingUnit(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcProtectiveDeviceTrippingUnit", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcProtectiveDeviceTrippingUnitType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcProtectiveDeviceTrippingUnitType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcProtectiveDeviceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcProtectiveDeviceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcProxy(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value ProxyType, Value Tag) { return ifcapi::express::make_entity("IFC4X1", "IfcProxy", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"ProxyType", ProxyType}, {"Tag", Tag}}); }
inline Value IfcPump(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcPump", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcPumpType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcPumpType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcQuantityArea(Value Name, Value Description, Value Unit, Value AreaValue, Value Formula) { return ifcapi::express::make_entity("IFC4X1", "IfcQuantityArea", {{"Name", Name}, {"Description", Description}, {"Unit", Unit}, {"AreaValue", AreaValue}, {"Formula", Formula}}); }
inline Value IfcQuantityCount(Value Name, Value Description, Value Unit, Value CountValue, Value Formula) { return ifcapi::express::make_entity("IFC4X1", "IfcQuantityCount", {{"Name", Name}, {"Description", Description}, {"Unit", Unit}, {"CountValue", CountValue}, {"Formula", Formula}}); }
inline Value IfcQuantityLength(Value Name, Value Description, Value Unit, Value LengthValue, Value Formula) { return ifcapi::express::make_entity("IFC4X1", "IfcQuantityLength", {{"Name", Name}, {"Description", Description}, {"Unit", Unit}, {"LengthValue", LengthValue}, {"Formula", Formula}}); }
inline Value IfcQuantitySet(Value GlobalId, Value OwnerHistory, Value Name, Value Description) { return ifcapi::express::make_entity("IFC4X1", "IfcQuantitySet", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}}); }
inline Value IfcQuantityTime(Value Name, Value Description, Value Unit, Value TimeValue, Value Formula) { return ifcapi::express::make_entity("IFC4X1", "IfcQuantityTime", {{"Name", Name}, {"Description", Description}, {"Unit", Unit}, {"TimeValue", TimeValue}, {"Formula", Formula}}); }
inline Value IfcQuantityVolume(Value Name, Value Description, Value Unit, Value VolumeValue, Value Formula) { return ifcapi::express::make_entity("IFC4X1", "IfcQuantityVolume", {{"Name", Name}, {"Description", Description}, {"Unit", Unit}, {"VolumeValue", VolumeValue}, {"Formula", Formula}}); }
inline Value IfcQuantityWeight(Value Name, Value Description, Value Unit, Value WeightValue, Value Formula) { return ifcapi::express::make_entity("IFC4X1", "IfcQuantityWeight", {{"Name", Name}, {"Description", Description}, {"Unit", Unit}, {"WeightValue", WeightValue}, {"Formula", Formula}}); }
inline Value IfcRailing(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcRailing", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcRailingType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcRailingType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcRamp(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcRamp", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcRampFlight(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcRampFlight", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcRampFlightType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcRampFlightType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcRampType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcRampType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcRationalBSplineCurveWithKnots(Value Degree, Value ControlPointsList, Value CurveForm, Value ClosedCurve, Value SelfIntersect, Value KnotMultiplicities, Value Knots, Value KnotSpec, Value WeightsData) { return ifcapi::express::make_entity("IFC4X1", "IfcRationalBSplineCurveWithKnots", {{"Degree", Degree}, {"ControlPointsList", ControlPointsList}, {"CurveForm", CurveForm}, {"ClosedCurve", ClosedCurve}, {"SelfIntersect", SelfIntersect}, {"KnotMultiplicities", KnotMultiplicities}, {"Knots", Knots}, {"KnotSpec", KnotSpec}, {"WeightsData", WeightsData}}); }
inline Value IfcRationalBSplineSurfaceWithKnots(Value UDegree, Value VDegree, Value ControlPointsList, Value SurfaceForm, Value UClosed, Value VClosed, Value SelfIntersect, Value UMultiplicities, Value VMultiplicities, Value UKnots, Value VKnots, Value KnotSpec, Value WeightsData) { return ifcapi::express::make_entity("IFC4X1", "IfcRationalBSplineSurfaceWithKnots", {{"UDegree", UDegree}, {"VDegree", VDegree}, {"ControlPointsList", ControlPointsList}, {"SurfaceForm", SurfaceForm}, {"UClosed", UClosed}, {"VClosed", VClosed}, {"SelfIntersect", SelfIntersect}, {"UMultiplicities", UMultiplicities}, {"VMultiplicities", VMultiplicities}, {"UKnots", UKnots}, {"VKnots", VKnots}, {"KnotSpec", KnotSpec}, {"WeightsData", WeightsData}}); }
inline Value IfcRectangleHollowProfileDef(Value ProfileType, Value ProfileName, Value Position, Value XDim, Value YDim, Value WallThickness, Value InnerFilletRadius, Value OuterFilletRadius) { return ifcapi::express::make_entity("IFC4X1", "IfcRectangleHollowProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"XDim", XDim}, {"YDim", YDim}, {"WallThickness", WallThickness}, {"InnerFilletRadius", InnerFilletRadius}, {"OuterFilletRadius", OuterFilletRadius}}); }
inline Value IfcRectangleProfileDef(Value ProfileType, Value ProfileName, Value Position, Value XDim, Value YDim) { return ifcapi::express::make_entity("IFC4X1", "IfcRectangleProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"XDim", XDim}, {"YDim", YDim}}); }
inline Value IfcRectangularPyramid(Value Position, Value XLength, Value YLength, Value Height) { return ifcapi::express::make_entity("IFC4X1", "IfcRectangularPyramid", {{"Position", Position}, {"XLength", XLength}, {"YLength", YLength}, {"Height", Height}}); }
inline Value IfcRectangularTrimmedSurface(Value BasisSurface, Value U1, Value V1, Value U2, Value V2, Value Usense, Value Vsense) { return ifcapi::express::make_entity("IFC4X1", "IfcRectangularTrimmedSurface", {{"BasisSurface", BasisSurface}, {"U1", U1}, {"V1", V1}, {"U2", U2}, {"V2", V2}, {"Usense", Usense}, {"Vsense", Vsense}}); }
inline Value IfcRecurrencePattern(Value RecurrenceType, Value DayComponent, Value WeekdayComponent, Value MonthComponent, Value Position, Value Interval, Value Occurrences, Value TimePeriods) { return ifcapi::express::make_entity("IFC4X1", "IfcRecurrencePattern", {{"RecurrenceType", RecurrenceType}, {"DayComponent", DayComponent}, {"WeekdayComponent", WeekdayComponent}, {"MonthComponent", MonthComponent}, {"Position", Position}, {"Interval", Interval}, {"Occurrences", Occurrences}, {"TimePeriods", TimePeriods}}); }
inline Value IfcReference(Value TypeIdentifier, Value AttributeIdentifier, Value InstanceName, Value ListPositions, Value InnerReference) { return ifcapi::express::make_entity("IFC4X1", "IfcReference", {{"TypeIdentifier", TypeIdentifier}, {"AttributeIdentifier", AttributeIdentifier}, {"InstanceName", InstanceName}, {"ListPositions", ListPositions}, {"InnerReference", InnerReference}}); }
inline Value IfcReferent(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value PredefinedType, Value RestartDistance) { return ifcapi::express::make_entity("IFC4X1", "IfcReferent", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"PredefinedType", PredefinedType}, {"RestartDistance", RestartDistance}}); }
inline Value IfcRegularTimeSeries(Value Name, Value Description, Value StartTime, Value EndTime, Value TimeSeriesDataType, Value DataOrigin, Value UserDefinedDataOrigin, Value Unit, Value TimeStep, Value Values) { return ifcapi::express::make_entity("IFC4X1", "IfcRegularTimeSeries", {{"Name", Name}, {"Description", Description}, {"StartTime", StartTime}, {"EndTime", EndTime}, {"TimeSeriesDataType", TimeSeriesDataType}, {"DataOrigin", DataOrigin}, {"UserDefinedDataOrigin", UserDefinedDataOrigin}, {"Unit", Unit}, {"TimeStep", TimeStep}, {"Values", Values}}); }
inline Value IfcReinforcementBarProperties(Value TotalCrossSectionArea, Value SteelGrade, Value BarSurface, Value EffectiveDepth, Value NominalBarDiameter, Value BarCount) { return ifcapi::express::make_entity("IFC4X1", "IfcReinforcementBarProperties", {{"TotalCrossSectionArea", TotalCrossSectionArea}, {"SteelGrade", SteelGrade}, {"BarSurface", BarSurface}, {"EffectiveDepth", EffectiveDepth}, {"NominalBarDiameter", NominalBarDiameter}, {"BarCount", BarCount}}); }
inline Value IfcReinforcementDefinitionProperties(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value DefinitionType, Value ReinforcementSectionDefinitions) { return ifcapi::express::make_entity("IFC4X1", "IfcReinforcementDefinitionProperties", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"DefinitionType", DefinitionType}, {"ReinforcementSectionDefinitions", ReinforcementSectionDefinitions}}); }
inline Value IfcReinforcingBar(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value SteelGrade, Value NominalDiameter, Value CrossSectionArea, Value BarLength, Value PredefinedType, Value BarSurface) { return ifcapi::express::make_entity("IFC4X1", "IfcReinforcingBar", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"SteelGrade", SteelGrade}, {"NominalDiameter", NominalDiameter}, {"CrossSectionArea", CrossSectionArea}, {"BarLength", BarLength}, {"PredefinedType", PredefinedType}, {"BarSurface", BarSurface}}); }
inline Value IfcReinforcingBarType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType, Value NominalDiameter, Value CrossSectionArea, Value BarLength, Value BarSurface, Value BendingShapeCode, Value BendingParameters) { return ifcapi::express::make_entity("IFC4X1", "IfcReinforcingBarType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}, {"NominalDiameter", NominalDiameter}, {"CrossSectionArea", CrossSectionArea}, {"BarLength", BarLength}, {"BarSurface", BarSurface}, {"BendingShapeCode", BendingShapeCode}, {"BendingParameters", BendingParameters}}); }
inline Value IfcReinforcingElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value SteelGrade) { return ifcapi::express::make_entity("IFC4X1", "IfcReinforcingElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"SteelGrade", SteelGrade}}); }
inline Value IfcReinforcingElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC4X1", "IfcReinforcingElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcReinforcingMesh(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value SteelGrade, Value MeshLength, Value MeshWidth, Value LongitudinalBarNominalDiameter, Value TransverseBarNominalDiameter, Value LongitudinalBarCrossSectionArea, Value TransverseBarCrossSectionArea, Value LongitudinalBarSpacing, Value TransverseBarSpacing, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcReinforcingMesh", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"SteelGrade", SteelGrade}, {"MeshLength", MeshLength}, {"MeshWidth", MeshWidth}, {"LongitudinalBarNominalDiameter", LongitudinalBarNominalDiameter}, {"TransverseBarNominalDiameter", TransverseBarNominalDiameter}, {"LongitudinalBarCrossSectionArea", LongitudinalBarCrossSectionArea}, {"TransverseBarCrossSectionArea", TransverseBarCrossSectionArea}, {"LongitudinalBarSpacing", LongitudinalBarSpacing}, {"TransverseBarSpacing", TransverseBarSpacing}, {"PredefinedType", PredefinedType}}); }
inline Value IfcReinforcingMeshType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType, Value MeshLength, Value MeshWidth, Value LongitudinalBarNominalDiameter, Value TransverseBarNominalDiameter, Value LongitudinalBarCrossSectionArea, Value TransverseBarCrossSectionArea, Value LongitudinalBarSpacing, Value TransverseBarSpacing, Value BendingShapeCode, Value BendingParameters) { return ifcapi::express::make_entity("IFC4X1", "IfcReinforcingMeshType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}, {"MeshLength", MeshLength}, {"MeshWidth", MeshWidth}, {"LongitudinalBarNominalDiameter", LongitudinalBarNominalDiameter}, {"TransverseBarNominalDiameter", TransverseBarNominalDiameter}, {"LongitudinalBarCrossSectionArea", LongitudinalBarCrossSectionArea}, {"TransverseBarCrossSectionArea", TransverseBarCrossSectionArea}, {"LongitudinalBarSpacing", LongitudinalBarSpacing}, {"TransverseBarSpacing", TransverseBarSpacing}, {"BendingShapeCode", BendingShapeCode}, {"BendingParameters", BendingParameters}}); }
inline Value IfcRelAggregates(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingObject, Value RelatedObjects) { return ifcapi::express::make_entity("IFC4X1", "IfcRelAggregates", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingObject", RelatingObject}, {"RelatedObjects", RelatedObjects}}); }
inline Value IfcRelAssigns(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatedObjectsType) { return ifcapi::express::make_entity("IFC4X1", "IfcRelAssigns", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatedObjectsType", RelatedObjectsType}}); }
inline Value IfcRelAssignsToActor(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatedObjectsType, Value RelatingActor, Value ActingRole) { return ifcapi::express::make_entity("IFC4X1", "IfcRelAssignsToActor", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatedObjectsType", RelatedObjectsType}, {"RelatingActor", RelatingActor}, {"ActingRole", ActingRole}}); }
inline Value IfcRelAssignsToControl(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatedObjectsType, Value RelatingControl) { return ifcapi::express::make_entity("IFC4X1", "IfcRelAssignsToControl", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatedObjectsType", RelatedObjectsType}, {"RelatingControl", RelatingControl}}); }
inline Value IfcRelAssignsToGroup(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatedObjectsType, Value RelatingGroup) { return ifcapi::express::make_entity("IFC4X1", "IfcRelAssignsToGroup", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatedObjectsType", RelatedObjectsType}, {"RelatingGroup", RelatingGroup}}); }
inline Value IfcRelAssignsToGroupByFactor(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatedObjectsType, Value RelatingGroup, Value Factor) { return ifcapi::express::make_entity("IFC4X1", "IfcRelAssignsToGroupByFactor", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatedObjectsType", RelatedObjectsType}, {"RelatingGroup", RelatingGroup}, {"Factor", Factor}}); }
inline Value IfcRelAssignsToProcess(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatedObjectsType, Value RelatingProcess, Value QuantityInProcess) { return ifcapi::express::make_entity("IFC4X1", "IfcRelAssignsToProcess", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatedObjectsType", RelatedObjectsType}, {"RelatingProcess", RelatingProcess}, {"QuantityInProcess", QuantityInProcess}}); }
inline Value IfcRelAssignsToProduct(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatedObjectsType, Value RelatingProduct) { return ifcapi::express::make_entity("IFC4X1", "IfcRelAssignsToProduct", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatedObjectsType", RelatedObjectsType}, {"RelatingProduct", RelatingProduct}}); }
inline Value IfcRelAssignsToResource(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatedObjectsType, Value RelatingResource) { return ifcapi::express::make_entity("IFC4X1", "IfcRelAssignsToResource", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatedObjectsType", RelatedObjectsType}, {"RelatingResource", RelatingResource}}); }
inline Value IfcRelAssociates(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects) { return ifcapi::express::make_entity("IFC4X1", "IfcRelAssociates", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}}); }
inline Value IfcRelAssociatesApproval(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatingApproval) { return ifcapi::express::make_entity("IFC4X1", "IfcRelAssociatesApproval", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatingApproval", RelatingApproval}}); }
inline Value IfcRelAssociatesClassification(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatingClassification) { return ifcapi::express::make_entity("IFC4X1", "IfcRelAssociatesClassification", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatingClassification", RelatingClassification}}); }
inline Value IfcRelAssociatesConstraint(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value Intent, Value RelatingConstraint) { return ifcapi::express::make_entity("IFC4X1", "IfcRelAssociatesConstraint", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"Intent", Intent}, {"RelatingConstraint", RelatingConstraint}}); }
inline Value IfcRelAssociatesDocument(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatingDocument) { return ifcapi::express::make_entity("IFC4X1", "IfcRelAssociatesDocument", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatingDocument", RelatingDocument}}); }
inline Value IfcRelAssociatesLibrary(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatingLibrary) { return ifcapi::express::make_entity("IFC4X1", "IfcRelAssociatesLibrary", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatingLibrary", RelatingLibrary}}); }
inline Value IfcRelAssociatesMaterial(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatingMaterial) { return ifcapi::express::make_entity("IFC4X1", "IfcRelAssociatesMaterial", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatingMaterial", RelatingMaterial}}); }
inline Value IfcRelConnects(Value GlobalId, Value OwnerHistory, Value Name, Value Description) { return ifcapi::express::make_entity("IFC4X1", "IfcRelConnects", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}}); }
inline Value IfcRelConnectsElements(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ConnectionGeometry, Value RelatingElement, Value RelatedElement) { return ifcapi::express::make_entity("IFC4X1", "IfcRelConnectsElements", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ConnectionGeometry", ConnectionGeometry}, {"RelatingElement", RelatingElement}, {"RelatedElement", RelatedElement}}); }
inline Value IfcRelConnectsPathElements(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ConnectionGeometry, Value RelatingElement, Value RelatedElement, Value RelatingPriorities, Value RelatedPriorities, Value RelatedConnectionType, Value RelatingConnectionType) { return ifcapi::express::make_entity("IFC4X1", "IfcRelConnectsPathElements", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ConnectionGeometry", ConnectionGeometry}, {"RelatingElement", RelatingElement}, {"RelatedElement", RelatedElement}, {"RelatingPriorities", RelatingPriorities}, {"RelatedPriorities", RelatedPriorities}, {"RelatedConnectionType", RelatedConnectionType}, {"RelatingConnectionType", RelatingConnectionType}}); }
inline Value IfcRelConnectsPortToElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingPort, Value RelatedElement) { return ifcapi::express::make_entity("IFC4X1", "IfcRelConnectsPortToElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingPort", RelatingPort}, {"RelatedElement", RelatedElement}}); }
inline Value IfcRelConnectsPorts(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingPort, Value RelatedPort, Value RealizingElement) { return ifcapi::express::make_entity("IFC4X1", "IfcRelConnectsPorts", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingPort", RelatingPort}, {"RelatedPort", RelatedPort}, {"RealizingElement", RealizingElement}}); }
inline Value IfcRelConnectsStructuralActivity(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingElement, Value RelatedStructuralActivity) { return ifcapi::express::make_entity("IFC4X1", "IfcRelConnectsStructuralActivity", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingElement", RelatingElement}, {"RelatedStructuralActivity", RelatedStructuralActivity}}); }
inline Value IfcRelConnectsStructuralMember(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingStructuralMember, Value RelatedStructuralConnection, Value AppliedCondition, Value AdditionalConditions, Value SupportedLength, Value ConditionCoordinateSystem) { return ifcapi::express::make_entity("IFC4X1", "IfcRelConnectsStructuralMember", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingStructuralMember", RelatingStructuralMember}, {"RelatedStructuralConnection", RelatedStructuralConnection}, {"AppliedCondition", AppliedCondition}, {"AdditionalConditions", AdditionalConditions}, {"SupportedLength", SupportedLength}, {"ConditionCoordinateSystem", ConditionCoordinateSystem}}); }
inline Value IfcRelConnectsWithEccentricity(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingStructuralMember, Value RelatedStructuralConnection, Value AppliedCondition, Value AdditionalConditions, Value SupportedLength, Value ConditionCoordinateSystem, Value ConnectionConstraint) { return ifcapi::express::make_entity("IFC4X1", "IfcRelConnectsWithEccentricity", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingStructuralMember", RelatingStructuralMember}, {"RelatedStructuralConnection", RelatedStructuralConnection}, {"AppliedCondition", AppliedCondition}, {"AdditionalConditions", AdditionalConditions}, {"SupportedLength", SupportedLength}, {"ConditionCoordinateSystem", ConditionCoordinateSystem}, {"ConnectionConstraint", ConnectionConstraint}}); }
inline Value IfcRelConnectsWithRealizingElements(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ConnectionGeometry, Value RelatingElement, Value RelatedElement, Value RealizingElements, Value ConnectionType) { return ifcapi::express::make_entity("IFC4X1", "IfcRelConnectsWithRealizingElements", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ConnectionGeometry", ConnectionGeometry}, {"RelatingElement", RelatingElement}, {"RelatedElement", RelatedElement}, {"RealizingElements", RealizingElements}, {"ConnectionType", ConnectionType}}); }
inline Value IfcRelContainedInSpatialStructure(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedElements, Value RelatingStructure) { return ifcapi::express::make_entity("IFC4X1", "IfcRelContainedInSpatialStructure", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedElements", RelatedElements}, {"RelatingStructure", RelatingStructure}}); }
inline Value IfcRelCoversBldgElements(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingBuildingElement, Value RelatedCoverings) { return ifcapi::express::make_entity("IFC4X1", "IfcRelCoversBldgElements", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingBuildingElement", RelatingBuildingElement}, {"RelatedCoverings", RelatedCoverings}}); }
inline Value IfcRelCoversSpaces(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingSpace, Value RelatedCoverings) { return ifcapi::express::make_entity("IFC4X1", "IfcRelCoversSpaces", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingSpace", RelatingSpace}, {"RelatedCoverings", RelatedCoverings}}); }
inline Value IfcRelDeclares(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingContext, Value RelatedDefinitions) { return ifcapi::express::make_entity("IFC4X1", "IfcRelDeclares", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingContext", RelatingContext}, {"RelatedDefinitions", RelatedDefinitions}}); }
inline Value IfcRelDecomposes(Value GlobalId, Value OwnerHistory, Value Name, Value Description) { return ifcapi::express::make_entity("IFC4X1", "IfcRelDecomposes", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}}); }
inline Value IfcRelDefines(Value GlobalId, Value OwnerHistory, Value Name, Value Description) { return ifcapi::express::make_entity("IFC4X1", "IfcRelDefines", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}}); }
inline Value IfcRelDefinesByObject(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatingObject) { return ifcapi::express::make_entity("IFC4X1", "IfcRelDefinesByObject", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatingObject", RelatingObject}}); }
inline Value IfcRelDefinesByProperties(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatingPropertyDefinition) { return ifcapi::express::make_entity("IFC4X1", "IfcRelDefinesByProperties", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatingPropertyDefinition", RelatingPropertyDefinition}}); }
inline Value IfcRelDefinesByTemplate(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedPropertySets, Value RelatingTemplate) { return ifcapi::express::make_entity("IFC4X1", "IfcRelDefinesByTemplate", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedPropertySets", RelatedPropertySets}, {"RelatingTemplate", RelatingTemplate}}); }
inline Value IfcRelDefinesByType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatingType) { return ifcapi::express::make_entity("IFC4X1", "IfcRelDefinesByType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatingType", RelatingType}}); }
inline Value IfcRelFillsElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingOpeningElement, Value RelatedBuildingElement) { return ifcapi::express::make_entity("IFC4X1", "IfcRelFillsElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingOpeningElement", RelatingOpeningElement}, {"RelatedBuildingElement", RelatedBuildingElement}}); }
inline Value IfcRelFlowControlElements(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedControlElements, Value RelatingFlowElement) { return ifcapi::express::make_entity("IFC4X1", "IfcRelFlowControlElements", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedControlElements", RelatedControlElements}, {"RelatingFlowElement", RelatingFlowElement}}); }
inline Value IfcRelInterferesElements(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingElement, Value RelatedElement, Value InterferenceGeometry, Value InterferenceType, Value ImpliedOrder) { return ifcapi::express::make_entity("IFC4X1", "IfcRelInterferesElements", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingElement", RelatingElement}, {"RelatedElement", RelatedElement}, {"InterferenceGeometry", InterferenceGeometry}, {"InterferenceType", InterferenceType}, {"ImpliedOrder", ImpliedOrder}}); }
inline Value IfcRelNests(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingObject, Value RelatedObjects) { return ifcapi::express::make_entity("IFC4X1", "IfcRelNests", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingObject", RelatingObject}, {"RelatedObjects", RelatedObjects}}); }
inline Value IfcRelProjectsElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingElement, Value RelatedFeatureElement) { return ifcapi::express::make_entity("IFC4X1", "IfcRelProjectsElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingElement", RelatingElement}, {"RelatedFeatureElement", RelatedFeatureElement}}); }
inline Value IfcRelReferencedInSpatialStructure(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedElements, Value RelatingStructure) { return ifcapi::express::make_entity("IFC4X1", "IfcRelReferencedInSpatialStructure", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedElements", RelatedElements}, {"RelatingStructure", RelatingStructure}}); }
inline Value IfcRelSequence(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingProcess, Value RelatedProcess, Value TimeLag, Value SequenceType, Value UserDefinedSequenceType) { return ifcapi::express::make_entity("IFC4X1", "IfcRelSequence", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingProcess", RelatingProcess}, {"RelatedProcess", RelatedProcess}, {"TimeLag", TimeLag}, {"SequenceType", SequenceType}, {"UserDefinedSequenceType", UserDefinedSequenceType}}); }
inline Value IfcRelServicesBuildings(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingSystem, Value RelatedBuildings) { return ifcapi::express::make_entity("IFC4X1", "IfcRelServicesBuildings", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingSystem", RelatingSystem}, {"RelatedBuildings", RelatedBuildings}}); }
inline Value IfcRelSpaceBoundary(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingSpace, Value RelatedBuildingElement, Value ConnectionGeometry, Value PhysicalOrVirtualBoundary, Value InternalOrExternalBoundary) { return ifcapi::express::make_entity("IFC4X1", "IfcRelSpaceBoundary", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingSpace", RelatingSpace}, {"RelatedBuildingElement", RelatedBuildingElement}, {"ConnectionGeometry", ConnectionGeometry}, {"PhysicalOrVirtualBoundary", PhysicalOrVirtualBoundary}, {"InternalOrExternalBoundary", InternalOrExternalBoundary}}); }
inline Value IfcRelSpaceBoundary1stLevel(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingSpace, Value RelatedBuildingElement, Value ConnectionGeometry, Value PhysicalOrVirtualBoundary, Value InternalOrExternalBoundary, Value ParentBoundary) { return ifcapi::express::make_entity("IFC4X1", "IfcRelSpaceBoundary1stLevel", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingSpace", RelatingSpace}, {"RelatedBuildingElement", RelatedBuildingElement}, {"ConnectionGeometry", ConnectionGeometry}, {"PhysicalOrVirtualBoundary", PhysicalOrVirtualBoundary}, {"InternalOrExternalBoundary", InternalOrExternalBoundary}, {"ParentBoundary", ParentBoundary}}); }
inline Value IfcRelSpaceBoundary2ndLevel(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingSpace, Value RelatedBuildingElement, Value ConnectionGeometry, Value PhysicalOrVirtualBoundary, Value InternalOrExternalBoundary, Value ParentBoundary, Value CorrespondingBoundary) { return ifcapi::express::make_entity("IFC4X1", "IfcRelSpaceBoundary2ndLevel", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingSpace", RelatingSpace}, {"RelatedBuildingElement", RelatedBuildingElement}, {"ConnectionGeometry", ConnectionGeometry}, {"PhysicalOrVirtualBoundary", PhysicalOrVirtualBoundary}, {"InternalOrExternalBoundary", InternalOrExternalBoundary}, {"ParentBoundary", ParentBoundary}, {"CorrespondingBoundary", CorrespondingBoundary}}); }
inline Value IfcRelVoidsElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingBuildingElement, Value RelatedOpeningElement) { return ifcapi::express::make_entity("IFC4X1", "IfcRelVoidsElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingBuildingElement", RelatingBuildingElement}, {"RelatedOpeningElement", RelatedOpeningElement}}); }
inline Value IfcRelationship(Value GlobalId, Value OwnerHistory, Value Name, Value Description) { return ifcapi::express::make_entity("IFC4X1", "IfcRelationship", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}}); }
inline Value IfcReparametrisedCompositeCurveSegment(Value Transition, Value SameSense, Value ParentCurve, Value ParamLength) { return ifcapi::express::make_entity("IFC4X1", "IfcReparametrisedCompositeCurveSegment", {{"Transition", Transition}, {"SameSense", SameSense}, {"ParentCurve", ParentCurve}, {"ParamLength", ParamLength}}); }
inline Value IfcRepresentation(Value ContextOfItems, Value RepresentationIdentifier, Value RepresentationType, Value Items) { return ifcapi::express::make_entity("IFC4X1", "IfcRepresentation", {{"ContextOfItems", ContextOfItems}, {"RepresentationIdentifier", RepresentationIdentifier}, {"RepresentationType", RepresentationType}, {"Items", Items}}); }
inline Value IfcRepresentationContext(Value ContextIdentifier, Value ContextType) { return ifcapi::express::make_entity("IFC4X1", "IfcRepresentationContext", {{"ContextIdentifier", ContextIdentifier}, {"ContextType", ContextType}}); }
inline Value IfcRepresentationItem() { return ifcapi::express::make_entity("IFC4X1", "IfcRepresentationItem", {}); }
inline Value IfcRepresentationMap(Value MappingOrigin, Value MappedRepresentation) { return ifcapi::express::make_entity("IFC4X1", "IfcRepresentationMap", {{"MappingOrigin", MappingOrigin}, {"MappedRepresentation", MappedRepresentation}}); }
inline Value IfcResource(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identification, Value LongDescription) { return ifcapi::express::make_entity("IFC4X1", "IfcResource", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identification", Identification}, {"LongDescription", LongDescription}}); }
inline Value IfcResourceApprovalRelationship(Value Name, Value Description, Value RelatedResourceObjects, Value RelatingApproval) { return ifcapi::express::make_entity("IFC4X1", "IfcResourceApprovalRelationship", {{"Name", Name}, {"Description", Description}, {"RelatedResourceObjects", RelatedResourceObjects}, {"RelatingApproval", RelatingApproval}}); }
inline Value IfcResourceConstraintRelationship(Value Name, Value Description, Value RelatingConstraint, Value RelatedResourceObjects) { return ifcapi::express::make_entity("IFC4X1", "IfcResourceConstraintRelationship", {{"Name", Name}, {"Description", Description}, {"RelatingConstraint", RelatingConstraint}, {"RelatedResourceObjects", RelatedResourceObjects}}); }
inline Value IfcResourceLevelRelationship(Value Name, Value Description) { return ifcapi::express::make_entity("IFC4X1", "IfcResourceLevelRelationship", {{"Name", Name}, {"Description", Description}}); }
inline Value IfcResourceTime(Value Name, Value DataOrigin, Value UserDefinedDataOrigin, Value ScheduleWork, Value ScheduleUsage, Value ScheduleStart, Value ScheduleFinish, Value ScheduleContour, Value LevelingDelay, Value IsOverAllocated, Value StatusTime, Value ActualWork, Value ActualUsage, Value ActualStart, Value ActualFinish, Value RemainingWork, Value RemainingUsage, Value Completion) { return ifcapi::express::make_entity("IFC4X1", "IfcResourceTime", {{"Name", Name}, {"DataOrigin", DataOrigin}, {"UserDefinedDataOrigin", UserDefinedDataOrigin}, {"ScheduleWork", ScheduleWork}, {"ScheduleUsage", ScheduleUsage}, {"ScheduleStart", ScheduleStart}, {"ScheduleFinish", ScheduleFinish}, {"ScheduleContour", ScheduleContour}, {"LevelingDelay", LevelingDelay}, {"IsOverAllocated", IsOverAllocated}, {"StatusTime", StatusTime}, {"ActualWork", ActualWork}, {"ActualUsage", ActualUsage}, {"ActualStart", ActualStart}, {"ActualFinish", ActualFinish}, {"RemainingWork", RemainingWork}, {"RemainingUsage", RemainingUsage}, {"Completion", Completion}}); }
inline Value IfcRevolvedAreaSolid(Value SweptArea, Value Position, Value Axis, Value Angle) { return ifcapi::express::make_entity("IFC4X1", "IfcRevolvedAreaSolid", {{"SweptArea", SweptArea}, {"Position", Position}, {"Axis", Axis}, {"Angle", Angle}}); }
inline Value IfcRevolvedAreaSolidTapered(Value SweptArea, Value Position, Value Axis, Value Angle, Value EndSweptArea) { return ifcapi::express::make_entity("IFC4X1", "IfcRevolvedAreaSolidTapered", {{"SweptArea", SweptArea}, {"Position", Position}, {"Axis", Axis}, {"Angle", Angle}, {"EndSweptArea", EndSweptArea}}); }
inline Value IfcRightCircularCone(Value Position, Value Height, Value BottomRadius) { return ifcapi::express::make_entity("IFC4X1", "IfcRightCircularCone", {{"Position", Position}, {"Height", Height}, {"BottomRadius", BottomRadius}}); }
inline Value IfcRightCircularCylinder(Value Position, Value Height, Value Radius) { return ifcapi::express::make_entity("IFC4X1", "IfcRightCircularCylinder", {{"Position", Position}, {"Height", Height}, {"Radius", Radius}}); }
inline Value IfcRoof(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcRoof", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcRoofType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcRoofType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcRoot(Value GlobalId, Value OwnerHistory, Value Name, Value Description) { return ifcapi::express::make_entity("IFC4X1", "IfcRoot", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}}); }
inline Value IfcRoundedRectangleProfileDef(Value ProfileType, Value ProfileName, Value Position, Value XDim, Value YDim, Value RoundingRadius) { return ifcapi::express::make_entity("IFC4X1", "IfcRoundedRectangleProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"XDim", XDim}, {"YDim", YDim}, {"RoundingRadius", RoundingRadius}}); }
inline Value IfcSIUnit(Value Dimensions, Value UnitType, Value Prefix, Value Name) { return ifcapi::express::make_entity("IFC4X1", "IfcSIUnit", {{"Dimensions", Dimensions}, {"UnitType", UnitType}, {"Prefix", Prefix}, {"Name", Name}}); }
inline Value IfcSanitaryTerminal(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcSanitaryTerminal", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcSanitaryTerminalType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcSanitaryTerminalType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcSchedulingTime(Value Name, Value DataOrigin, Value UserDefinedDataOrigin) { return ifcapi::express::make_entity("IFC4X1", "IfcSchedulingTime", {{"Name", Name}, {"DataOrigin", DataOrigin}, {"UserDefinedDataOrigin", UserDefinedDataOrigin}}); }
inline Value IfcSeamCurve(Value Curve3D, Value AssociatedGeometry, Value MasterRepresentation) { return ifcapi::express::make_entity("IFC4X1", "IfcSeamCurve", {{"Curve3D", Curve3D}, {"AssociatedGeometry", AssociatedGeometry}, {"MasterRepresentation", MasterRepresentation}}); }
inline Value IfcSectionProperties(Value SectionType, Value StartProfile, Value EndProfile) { return ifcapi::express::make_entity("IFC4X1", "IfcSectionProperties", {{"SectionType", SectionType}, {"StartProfile", StartProfile}, {"EndProfile", EndProfile}}); }
inline Value IfcSectionReinforcementProperties(Value LongitudinalStartPosition, Value LongitudinalEndPosition, Value TransversePosition, Value ReinforcementRole, Value SectionDefinition, Value CrossSectionReinforcementDefinitions) { return ifcapi::express::make_entity("IFC4X1", "IfcSectionReinforcementProperties", {{"LongitudinalStartPosition", LongitudinalStartPosition}, {"LongitudinalEndPosition", LongitudinalEndPosition}, {"TransversePosition", TransversePosition}, {"ReinforcementRole", ReinforcementRole}, {"SectionDefinition", SectionDefinition}, {"CrossSectionReinforcementDefinitions", CrossSectionReinforcementDefinitions}}); }
inline Value IfcSectionedSolid(Value Directrix, Value CrossSections) { return ifcapi::express::make_entity("IFC4X1", "IfcSectionedSolid", {{"Directrix", Directrix}, {"CrossSections", CrossSections}}); }
inline Value IfcSectionedSolidHorizontal(Value Directrix, Value CrossSections, Value CrossSectionPositions, Value FixedAxisVertical) { return ifcapi::express::make_entity("IFC4X1", "IfcSectionedSolidHorizontal", {{"Directrix", Directrix}, {"CrossSections", CrossSections}, {"CrossSectionPositions", CrossSectionPositions}, {"FixedAxisVertical", FixedAxisVertical}}); }
inline Value IfcSectionedSpine(Value SpineCurve, Value CrossSections, Value CrossSectionPositions) { return ifcapi::express::make_entity("IFC4X1", "IfcSectionedSpine", {{"SpineCurve", SpineCurve}, {"CrossSections", CrossSections}, {"CrossSectionPositions", CrossSectionPositions}}); }
inline Value IfcSensor(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcSensor", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcSensorType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcSensorType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcShadingDevice(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcShadingDevice", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcShadingDeviceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcShadingDeviceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcShapeAspect(Value ShapeRepresentations, Value Name, Value Description, Value ProductDefinitional, Value PartOfProductDefinitionShape) { return ifcapi::express::make_entity("IFC4X1", "IfcShapeAspect", {{"ShapeRepresentations", ShapeRepresentations}, {"Name", Name}, {"Description", Description}, {"ProductDefinitional", ProductDefinitional}, {"PartOfProductDefinitionShape", PartOfProductDefinitionShape}}); }
inline Value IfcShapeModel(Value ContextOfItems, Value RepresentationIdentifier, Value RepresentationType, Value Items) { return ifcapi::express::make_entity("IFC4X1", "IfcShapeModel", {{"ContextOfItems", ContextOfItems}, {"RepresentationIdentifier", RepresentationIdentifier}, {"RepresentationType", RepresentationType}, {"Items", Items}}); }
inline Value IfcShapeRepresentation(Value ContextOfItems, Value RepresentationIdentifier, Value RepresentationType, Value Items) { return ifcapi::express::make_entity("IFC4X1", "IfcShapeRepresentation", {{"ContextOfItems", ContextOfItems}, {"RepresentationIdentifier", RepresentationIdentifier}, {"RepresentationType", RepresentationType}, {"Items", Items}}); }
inline Value IfcShellBasedSurfaceModel(Value SbsmBoundary) { return ifcapi::express::make_entity("IFC4X1", "IfcShellBasedSurfaceModel", {{"SbsmBoundary", SbsmBoundary}}); }
inline Value IfcSimpleProperty(Value Name, Value Description) { return ifcapi::express::make_entity("IFC4X1", "IfcSimpleProperty", {{"Name", Name}, {"Description", Description}}); }
inline Value IfcSimplePropertyTemplate(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value TemplateType, Value PrimaryMeasureType, Value SecondaryMeasureType, Value Enumerators, Value PrimaryUnit, Value SecondaryUnit, Value Expression, Value AccessState) { return ifcapi::express::make_entity("IFC4X1", "IfcSimplePropertyTemplate", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"TemplateType", TemplateType}, {"PrimaryMeasureType", PrimaryMeasureType}, {"SecondaryMeasureType", SecondaryMeasureType}, {"Enumerators", Enumerators}, {"PrimaryUnit", PrimaryUnit}, {"SecondaryUnit", SecondaryUnit}, {"Expression", Expression}, {"AccessState", AccessState}}); }
inline Value IfcSite(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value LongName, Value CompositionType, Value RefLatitude, Value RefLongitude, Value RefElevation, Value LandTitleNumber, Value SiteAddress) { return ifcapi::express::make_entity("IFC4X1", "IfcSite", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"LongName", LongName}, {"CompositionType", CompositionType}, {"RefLatitude", RefLatitude}, {"RefLongitude", RefLongitude}, {"RefElevation", RefElevation}, {"LandTitleNumber", LandTitleNumber}, {"SiteAddress", SiteAddress}}); }
inline Value IfcSlab(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcSlab", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcSlabElementedCase(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcSlabElementedCase", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcSlabStandardCase(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcSlabStandardCase", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcSlabType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcSlabType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcSlippageConnectionCondition(Value Name, Value SlippageX, Value SlippageY, Value SlippageZ) { return ifcapi::express::make_entity("IFC4X1", "IfcSlippageConnectionCondition", {{"Name", Name}, {"SlippageX", SlippageX}, {"SlippageY", SlippageY}, {"SlippageZ", SlippageZ}}); }
inline Value IfcSolarDevice(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcSolarDevice", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcSolarDeviceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcSolarDeviceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcSolidModel() { return ifcapi::express::make_entity("IFC4X1", "IfcSolidModel", {}); }
inline Value IfcSpace(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value LongName, Value CompositionType, Value PredefinedType, Value ElevationWithFlooring) { return ifcapi::express::make_entity("IFC4X1", "IfcSpace", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"LongName", LongName}, {"CompositionType", CompositionType}, {"PredefinedType", PredefinedType}, {"ElevationWithFlooring", ElevationWithFlooring}}); }
inline Value IfcSpaceHeater(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcSpaceHeater", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcSpaceHeaterType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcSpaceHeaterType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcSpaceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType, Value LongName) { return ifcapi::express::make_entity("IFC4X1", "IfcSpaceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}, {"LongName", LongName}}); }
inline Value IfcSpatialElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value LongName) { return ifcapi::express::make_entity("IFC4X1", "IfcSpatialElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"LongName", LongName}}); }
inline Value IfcSpatialElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC4X1", "IfcSpatialElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcSpatialStructureElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value LongName, Value CompositionType) { return ifcapi::express::make_entity("IFC4X1", "IfcSpatialStructureElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"LongName", LongName}, {"CompositionType", CompositionType}}); }
inline Value IfcSpatialStructureElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC4X1", "IfcSpatialStructureElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcSpatialZone(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value LongName, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcSpatialZone", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"LongName", LongName}, {"PredefinedType", PredefinedType}}); }
inline Value IfcSpatialZoneType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType, Value LongName) { return ifcapi::express::make_entity("IFC4X1", "IfcSpatialZoneType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}, {"LongName", LongName}}); }
inline Value IfcSphere(Value Position, Value Radius) { return ifcapi::express::make_entity("IFC4X1", "IfcSphere", {{"Position", Position}, {"Radius", Radius}}); }
inline Value IfcSphericalSurface(Value Position, Value Radius) { return ifcapi::express::make_entity("IFC4X1", "IfcSphericalSurface", {{"Position", Position}, {"Radius", Radius}}); }
inline Value IfcStackTerminal(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcStackTerminal", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcStackTerminalType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcStackTerminalType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcStair(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcStair", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcStairFlight(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value NumberOfRisers, Value NumberOfTreads, Value RiserHeight, Value TreadLength, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcStairFlight", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"NumberOfRisers", NumberOfRisers}, {"NumberOfTreads", NumberOfTreads}, {"RiserHeight", RiserHeight}, {"TreadLength", TreadLength}, {"PredefinedType", PredefinedType}}); }
inline Value IfcStairFlightType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcStairFlightType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcStairType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcStairType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcStructuralAction(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedLoad, Value GlobalOrLocal, Value DestabilizingLoad) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralAction", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedLoad", AppliedLoad}, {"GlobalOrLocal", GlobalOrLocal}, {"DestabilizingLoad", DestabilizingLoad}}); }
inline Value IfcStructuralActivity(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedLoad, Value GlobalOrLocal) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralActivity", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedLoad", AppliedLoad}, {"GlobalOrLocal", GlobalOrLocal}}); }
inline Value IfcStructuralAnalysisModel(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value PredefinedType, Value OrientationOf2DPlane, Value LoadedBy, Value HasResults, Value SharedPlacement) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralAnalysisModel", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"PredefinedType", PredefinedType}, {"OrientationOf2DPlane", OrientationOf2DPlane}, {"LoadedBy", LoadedBy}, {"HasResults", HasResults}, {"SharedPlacement", SharedPlacement}}); }
inline Value IfcStructuralConnection(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedCondition) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralConnection", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedCondition", AppliedCondition}}); }
inline Value IfcStructuralConnectionCondition(Value Name) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralConnectionCondition", {{"Name", Name}}); }
inline Value IfcStructuralCurveAction(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedLoad, Value GlobalOrLocal, Value DestabilizingLoad, Value ProjectedOrTrue, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralCurveAction", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedLoad", AppliedLoad}, {"GlobalOrLocal", GlobalOrLocal}, {"DestabilizingLoad", DestabilizingLoad}, {"ProjectedOrTrue", ProjectedOrTrue}, {"PredefinedType", PredefinedType}}); }
inline Value IfcStructuralCurveConnection(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedCondition, Value Axis) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralCurveConnection", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedCondition", AppliedCondition}, {"Axis", Axis}}); }
inline Value IfcStructuralCurveMember(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value PredefinedType, Value Axis) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralCurveMember", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"PredefinedType", PredefinedType}, {"Axis", Axis}}); }
inline Value IfcStructuralCurveMemberVarying(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value PredefinedType, Value Axis) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralCurveMemberVarying", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"PredefinedType", PredefinedType}, {"Axis", Axis}}); }
inline Value IfcStructuralCurveReaction(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedLoad, Value GlobalOrLocal, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralCurveReaction", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedLoad", AppliedLoad}, {"GlobalOrLocal", GlobalOrLocal}, {"PredefinedType", PredefinedType}}); }
inline Value IfcStructuralItem(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralItem", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}}); }
inline Value IfcStructuralLinearAction(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedLoad, Value GlobalOrLocal, Value DestabilizingLoad, Value ProjectedOrTrue, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralLinearAction", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedLoad", AppliedLoad}, {"GlobalOrLocal", GlobalOrLocal}, {"DestabilizingLoad", DestabilizingLoad}, {"ProjectedOrTrue", ProjectedOrTrue}, {"PredefinedType", PredefinedType}}); }
inline Value IfcStructuralLoad(Value Name) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralLoad", {{"Name", Name}}); }
inline Value IfcStructuralLoadCase(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value PredefinedType, Value ActionType, Value ActionSource, Value Coefficient, Value Purpose, Value SelfWeightCoefficients) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralLoadCase", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"PredefinedType", PredefinedType}, {"ActionType", ActionType}, {"ActionSource", ActionSource}, {"Coefficient", Coefficient}, {"Purpose", Purpose}, {"SelfWeightCoefficients", SelfWeightCoefficients}}); }
inline Value IfcStructuralLoadConfiguration(Value Name, Value Values, Value Locations) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralLoadConfiguration", {{"Name", Name}, {"Values", Values}, {"Locations", Locations}}); }
inline Value IfcStructuralLoadGroup(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value PredefinedType, Value ActionType, Value ActionSource, Value Coefficient, Value Purpose) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralLoadGroup", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"PredefinedType", PredefinedType}, {"ActionType", ActionType}, {"ActionSource", ActionSource}, {"Coefficient", Coefficient}, {"Purpose", Purpose}}); }
inline Value IfcStructuralLoadLinearForce(Value Name, Value LinearForceX, Value LinearForceY, Value LinearForceZ, Value LinearMomentX, Value LinearMomentY, Value LinearMomentZ) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralLoadLinearForce", {{"Name", Name}, {"LinearForceX", LinearForceX}, {"LinearForceY", LinearForceY}, {"LinearForceZ", LinearForceZ}, {"LinearMomentX", LinearMomentX}, {"LinearMomentY", LinearMomentY}, {"LinearMomentZ", LinearMomentZ}}); }
inline Value IfcStructuralLoadOrResult(Value Name) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralLoadOrResult", {{"Name", Name}}); }
inline Value IfcStructuralLoadPlanarForce(Value Name, Value PlanarForceX, Value PlanarForceY, Value PlanarForceZ) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralLoadPlanarForce", {{"Name", Name}, {"PlanarForceX", PlanarForceX}, {"PlanarForceY", PlanarForceY}, {"PlanarForceZ", PlanarForceZ}}); }
inline Value IfcStructuralLoadSingleDisplacement(Value Name, Value DisplacementX, Value DisplacementY, Value DisplacementZ, Value RotationalDisplacementRX, Value RotationalDisplacementRY, Value RotationalDisplacementRZ) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralLoadSingleDisplacement", {{"Name", Name}, {"DisplacementX", DisplacementX}, {"DisplacementY", DisplacementY}, {"DisplacementZ", DisplacementZ}, {"RotationalDisplacementRX", RotationalDisplacementRX}, {"RotationalDisplacementRY", RotationalDisplacementRY}, {"RotationalDisplacementRZ", RotationalDisplacementRZ}}); }
inline Value IfcStructuralLoadSingleDisplacementDistortion(Value Name, Value DisplacementX, Value DisplacementY, Value DisplacementZ, Value RotationalDisplacementRX, Value RotationalDisplacementRY, Value RotationalDisplacementRZ, Value Distortion) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralLoadSingleDisplacementDistortion", {{"Name", Name}, {"DisplacementX", DisplacementX}, {"DisplacementY", DisplacementY}, {"DisplacementZ", DisplacementZ}, {"RotationalDisplacementRX", RotationalDisplacementRX}, {"RotationalDisplacementRY", RotationalDisplacementRY}, {"RotationalDisplacementRZ", RotationalDisplacementRZ}, {"Distortion", Distortion}}); }
inline Value IfcStructuralLoadSingleForce(Value Name, Value ForceX, Value ForceY, Value ForceZ, Value MomentX, Value MomentY, Value MomentZ) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralLoadSingleForce", {{"Name", Name}, {"ForceX", ForceX}, {"ForceY", ForceY}, {"ForceZ", ForceZ}, {"MomentX", MomentX}, {"MomentY", MomentY}, {"MomentZ", MomentZ}}); }
inline Value IfcStructuralLoadSingleForceWarping(Value Name, Value ForceX, Value ForceY, Value ForceZ, Value MomentX, Value MomentY, Value MomentZ, Value WarpingMoment) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralLoadSingleForceWarping", {{"Name", Name}, {"ForceX", ForceX}, {"ForceY", ForceY}, {"ForceZ", ForceZ}, {"MomentX", MomentX}, {"MomentY", MomentY}, {"MomentZ", MomentZ}, {"WarpingMoment", WarpingMoment}}); }
inline Value IfcStructuralLoadStatic(Value Name) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralLoadStatic", {{"Name", Name}}); }
inline Value IfcStructuralLoadTemperature(Value Name, Value DeltaTConstant, Value DeltaTY, Value DeltaTZ) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralLoadTemperature", {{"Name", Name}, {"DeltaTConstant", DeltaTConstant}, {"DeltaTY", DeltaTY}, {"DeltaTZ", DeltaTZ}}); }
inline Value IfcStructuralMember(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralMember", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}}); }
inline Value IfcStructuralPlanarAction(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedLoad, Value GlobalOrLocal, Value DestabilizingLoad, Value ProjectedOrTrue, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralPlanarAction", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedLoad", AppliedLoad}, {"GlobalOrLocal", GlobalOrLocal}, {"DestabilizingLoad", DestabilizingLoad}, {"ProjectedOrTrue", ProjectedOrTrue}, {"PredefinedType", PredefinedType}}); }
inline Value IfcStructuralPointAction(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedLoad, Value GlobalOrLocal, Value DestabilizingLoad) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralPointAction", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedLoad", AppliedLoad}, {"GlobalOrLocal", GlobalOrLocal}, {"DestabilizingLoad", DestabilizingLoad}}); }
inline Value IfcStructuralPointConnection(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedCondition, Value ConditionCoordinateSystem) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralPointConnection", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedCondition", AppliedCondition}, {"ConditionCoordinateSystem", ConditionCoordinateSystem}}); }
inline Value IfcStructuralPointReaction(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedLoad, Value GlobalOrLocal) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralPointReaction", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedLoad", AppliedLoad}, {"GlobalOrLocal", GlobalOrLocal}}); }
inline Value IfcStructuralReaction(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedLoad, Value GlobalOrLocal) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralReaction", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedLoad", AppliedLoad}, {"GlobalOrLocal", GlobalOrLocal}}); }
inline Value IfcStructuralResultGroup(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value TheoryType, Value ResultForLoadGroup, Value IsLinear) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralResultGroup", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"TheoryType", TheoryType}, {"ResultForLoadGroup", ResultForLoadGroup}, {"IsLinear", IsLinear}}); }
inline Value IfcStructuralSurfaceAction(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedLoad, Value GlobalOrLocal, Value DestabilizingLoad, Value ProjectedOrTrue, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralSurfaceAction", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedLoad", AppliedLoad}, {"GlobalOrLocal", GlobalOrLocal}, {"DestabilizingLoad", DestabilizingLoad}, {"ProjectedOrTrue", ProjectedOrTrue}, {"PredefinedType", PredefinedType}}); }
inline Value IfcStructuralSurfaceConnection(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedCondition) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralSurfaceConnection", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedCondition", AppliedCondition}}); }
inline Value IfcStructuralSurfaceMember(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value PredefinedType, Value Thickness) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralSurfaceMember", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"PredefinedType", PredefinedType}, {"Thickness", Thickness}}); }
inline Value IfcStructuralSurfaceMemberVarying(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value PredefinedType, Value Thickness) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralSurfaceMemberVarying", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"PredefinedType", PredefinedType}, {"Thickness", Thickness}}); }
inline Value IfcStructuralSurfaceReaction(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedLoad, Value GlobalOrLocal, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcStructuralSurfaceReaction", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedLoad", AppliedLoad}, {"GlobalOrLocal", GlobalOrLocal}, {"PredefinedType", PredefinedType}}); }
inline Value IfcStyleModel(Value ContextOfItems, Value RepresentationIdentifier, Value RepresentationType, Value Items) { return ifcapi::express::make_entity("IFC4X1", "IfcStyleModel", {{"ContextOfItems", ContextOfItems}, {"RepresentationIdentifier", RepresentationIdentifier}, {"RepresentationType", RepresentationType}, {"Items", Items}}); }
inline Value IfcStyledItem(Value Item, Value Styles, Value Name) { return ifcapi::express::make_entity("IFC4X1", "IfcStyledItem", {{"Item", Item}, {"Styles", Styles}, {"Name", Name}}); }
inline Value IfcStyledRepresentation(Value ContextOfItems, Value RepresentationIdentifier, Value RepresentationType, Value Items) { return ifcapi::express::make_entity("IFC4X1", "IfcStyledRepresentation", {{"ContextOfItems", ContextOfItems}, {"RepresentationIdentifier", RepresentationIdentifier}, {"RepresentationType", RepresentationType}, {"Items", Items}}); }
inline Value IfcSubContractResource(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identification, Value LongDescription, Value Usage, Value BaseCosts, Value BaseQuantity, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcSubContractResource", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identification", Identification}, {"LongDescription", LongDescription}, {"Usage", Usage}, {"BaseCosts", BaseCosts}, {"BaseQuantity", BaseQuantity}, {"PredefinedType", PredefinedType}}); }
inline Value IfcSubContractResourceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value Identification, Value LongDescription, Value ResourceType, Value BaseCosts, Value BaseQuantity, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcSubContractResourceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"Identification", Identification}, {"LongDescription", LongDescription}, {"ResourceType", ResourceType}, {"BaseCosts", BaseCosts}, {"BaseQuantity", BaseQuantity}, {"PredefinedType", PredefinedType}}); }
inline Value IfcSubedge(Value EdgeStart, Value EdgeEnd, Value ParentEdge) { return ifcapi::express::make_entity("IFC4X1", "IfcSubedge", {{"EdgeStart", EdgeStart}, {"EdgeEnd", EdgeEnd}, {"ParentEdge", ParentEdge}}); }
inline Value IfcSurface() { return ifcapi::express::make_entity("IFC4X1", "IfcSurface", {}); }
inline Value IfcSurfaceCurve(Value Curve3D, Value AssociatedGeometry, Value MasterRepresentation) { return ifcapi::express::make_entity("IFC4X1", "IfcSurfaceCurve", {{"Curve3D", Curve3D}, {"AssociatedGeometry", AssociatedGeometry}, {"MasterRepresentation", MasterRepresentation}}); }
inline Value IfcSurfaceCurveSweptAreaSolid(Value SweptArea, Value Position, Value Directrix, Value StartParam, Value EndParam, Value ReferenceSurface) { return ifcapi::express::make_entity("IFC4X1", "IfcSurfaceCurveSweptAreaSolid", {{"SweptArea", SweptArea}, {"Position", Position}, {"Directrix", Directrix}, {"StartParam", StartParam}, {"EndParam", EndParam}, {"ReferenceSurface", ReferenceSurface}}); }
inline Value IfcSurfaceFeature(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcSurfaceFeature", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcSurfaceOfLinearExtrusion(Value SweptCurve, Value Position, Value ExtrudedDirection, Value Depth) { return ifcapi::express::make_entity("IFC4X1", "IfcSurfaceOfLinearExtrusion", {{"SweptCurve", SweptCurve}, {"Position", Position}, {"ExtrudedDirection", ExtrudedDirection}, {"Depth", Depth}}); }
inline Value IfcSurfaceOfRevolution(Value SweptCurve, Value Position, Value AxisPosition) { return ifcapi::express::make_entity("IFC4X1", "IfcSurfaceOfRevolution", {{"SweptCurve", SweptCurve}, {"Position", Position}, {"AxisPosition", AxisPosition}}); }
inline Value IfcSurfaceReinforcementArea(Value Name, Value SurfaceReinforcement1, Value SurfaceReinforcement2, Value ShearReinforcement) { return ifcapi::express::make_entity("IFC4X1", "IfcSurfaceReinforcementArea", {{"Name", Name}, {"SurfaceReinforcement1", SurfaceReinforcement1}, {"SurfaceReinforcement2", SurfaceReinforcement2}, {"ShearReinforcement", ShearReinforcement}}); }
inline Value IfcSurfaceStyle(Value Name, Value Side, Value Styles) { return ifcapi::express::make_entity("IFC4X1", "IfcSurfaceStyle", {{"Name", Name}, {"Side", Side}, {"Styles", Styles}}); }
inline Value IfcSurfaceStyleLighting(Value DiffuseTransmissionColour, Value DiffuseReflectionColour, Value TransmissionColour, Value ReflectanceColour) { return ifcapi::express::make_entity("IFC4X1", "IfcSurfaceStyleLighting", {{"DiffuseTransmissionColour", DiffuseTransmissionColour}, {"DiffuseReflectionColour", DiffuseReflectionColour}, {"TransmissionColour", TransmissionColour}, {"ReflectanceColour", ReflectanceColour}}); }
inline Value IfcSurfaceStyleRefraction(Value RefractionIndex, Value DispersionFactor) { return ifcapi::express::make_entity("IFC4X1", "IfcSurfaceStyleRefraction", {{"RefractionIndex", RefractionIndex}, {"DispersionFactor", DispersionFactor}}); }
inline Value IfcSurfaceStyleRendering(Value SurfaceColour, Value Transparency, Value DiffuseColour, Value TransmissionColour, Value DiffuseTransmissionColour, Value ReflectionColour, Value SpecularColour, Value SpecularHighlight, Value ReflectanceMethod) { return ifcapi::express::make_entity("IFC4X1", "IfcSurfaceStyleRendering", {{"SurfaceColour", SurfaceColour}, {"Transparency", Transparency}, {"DiffuseColour", DiffuseColour}, {"TransmissionColour", TransmissionColour}, {"DiffuseTransmissionColour", DiffuseTransmissionColour}, {"ReflectionColour", ReflectionColour}, {"SpecularColour", SpecularColour}, {"SpecularHighlight", SpecularHighlight}, {"ReflectanceMethod", ReflectanceMethod}}); }
inline Value IfcSurfaceStyleShading(Value SurfaceColour, Value Transparency) { return ifcapi::express::make_entity("IFC4X1", "IfcSurfaceStyleShading", {{"SurfaceColour", SurfaceColour}, {"Transparency", Transparency}}); }
inline Value IfcSurfaceStyleWithTextures(Value Textures) { return ifcapi::express::make_entity("IFC4X1", "IfcSurfaceStyleWithTextures", {{"Textures", Textures}}); }
inline Value IfcSurfaceTexture(Value RepeatS, Value RepeatT, Value Mode, Value TextureTransform, Value Parameter) { return ifcapi::express::make_entity("IFC4X1", "IfcSurfaceTexture", {{"RepeatS", RepeatS}, {"RepeatT", RepeatT}, {"Mode", Mode}, {"TextureTransform", TextureTransform}, {"Parameter", Parameter}}); }
inline Value IfcSweptAreaSolid(Value SweptArea, Value Position) { return ifcapi::express::make_entity("IFC4X1", "IfcSweptAreaSolid", {{"SweptArea", SweptArea}, {"Position", Position}}); }
inline Value IfcSweptDiskSolid(Value Directrix, Value Radius, Value InnerRadius, Value StartParam, Value EndParam) { return ifcapi::express::make_entity("IFC4X1", "IfcSweptDiskSolid", {{"Directrix", Directrix}, {"Radius", Radius}, {"InnerRadius", InnerRadius}, {"StartParam", StartParam}, {"EndParam", EndParam}}); }
inline Value IfcSweptDiskSolidPolygonal(Value Directrix, Value Radius, Value InnerRadius, Value StartParam, Value EndParam, Value FilletRadius) { return ifcapi::express::make_entity("IFC4X1", "IfcSweptDiskSolidPolygonal", {{"Directrix", Directrix}, {"Radius", Radius}, {"InnerRadius", InnerRadius}, {"StartParam", StartParam}, {"EndParam", EndParam}, {"FilletRadius", FilletRadius}}); }
inline Value IfcSweptSurface(Value SweptCurve, Value Position) { return ifcapi::express::make_entity("IFC4X1", "IfcSweptSurface", {{"SweptCurve", SweptCurve}, {"Position", Position}}); }
inline Value IfcSwitchingDevice(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcSwitchingDevice", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcSwitchingDeviceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcSwitchingDeviceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcSystem(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType) { return ifcapi::express::make_entity("IFC4X1", "IfcSystem", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}}); }
inline Value IfcSystemFurnitureElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcSystemFurnitureElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcSystemFurnitureElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcSystemFurnitureElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcTShapeProfileDef(Value ProfileType, Value ProfileName, Value Position, Value Depth, Value FlangeWidth, Value WebThickness, Value FlangeThickness, Value FilletRadius, Value FlangeEdgeRadius, Value WebEdgeRadius, Value WebSlope, Value FlangeSlope) { return ifcapi::express::make_entity("IFC4X1", "IfcTShapeProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"Depth", Depth}, {"FlangeWidth", FlangeWidth}, {"WebThickness", WebThickness}, {"FlangeThickness", FlangeThickness}, {"FilletRadius", FilletRadius}, {"FlangeEdgeRadius", FlangeEdgeRadius}, {"WebEdgeRadius", WebEdgeRadius}, {"WebSlope", WebSlope}, {"FlangeSlope", FlangeSlope}}); }
inline Value IfcTable(Value Name, Value Rows, Value Columns) { return ifcapi::express::make_entity("IFC4X1", "IfcTable", {{"Name", Name}, {"Rows", Rows}, {"Columns", Columns}}); }
inline Value IfcTableColumn(Value Identifier, Value Name, Value Description, Value Unit, Value ReferencePath) { return ifcapi::express::make_entity("IFC4X1", "IfcTableColumn", {{"Identifier", Identifier}, {"Name", Name}, {"Description", Description}, {"Unit", Unit}, {"ReferencePath", ReferencePath}}); }
inline Value IfcTableRow(Value RowCells, Value IsHeading) { return ifcapi::express::make_entity("IFC4X1", "IfcTableRow", {{"RowCells", RowCells}, {"IsHeading", IsHeading}}); }
inline Value IfcTank(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcTank", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcTankType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcTankType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcTask(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identification, Value LongDescription, Value Status, Value WorkMethod, Value IsMilestone, Value Priority, Value TaskTime, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcTask", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identification", Identification}, {"LongDescription", LongDescription}, {"Status", Status}, {"WorkMethod", WorkMethod}, {"IsMilestone", IsMilestone}, {"Priority", Priority}, {"TaskTime", TaskTime}, {"PredefinedType", PredefinedType}}); }
inline Value IfcTaskTime(Value Name, Value DataOrigin, Value UserDefinedDataOrigin, Value DurationType, Value ScheduleDuration, Value ScheduleStart, Value ScheduleFinish, Value EarlyStart, Value EarlyFinish, Value LateStart, Value LateFinish, Value FreeFloat, Value TotalFloat, Value IsCritical, Value StatusTime, Value ActualDuration, Value ActualStart, Value ActualFinish, Value RemainingTime, Value Completion) { return ifcapi::express::make_entity("IFC4X1", "IfcTaskTime", {{"Name", Name}, {"DataOrigin", DataOrigin}, {"UserDefinedDataOrigin", UserDefinedDataOrigin}, {"DurationType", DurationType}, {"ScheduleDuration", ScheduleDuration}, {"ScheduleStart", ScheduleStart}, {"ScheduleFinish", ScheduleFinish}, {"EarlyStart", EarlyStart}, {"EarlyFinish", EarlyFinish}, {"LateStart", LateStart}, {"LateFinish", LateFinish}, {"FreeFloat", FreeFloat}, {"TotalFloat", TotalFloat}, {"IsCritical", IsCritical}, {"StatusTime", StatusTime}, {"ActualDuration", ActualDuration}, {"ActualStart", ActualStart}, {"ActualFinish", ActualFinish}, {"RemainingTime", RemainingTime}, {"Completion", Completion}}); }
inline Value IfcTaskTimeRecurring(Value Name, Value DataOrigin, Value UserDefinedDataOrigin, Value DurationType, Value ScheduleDuration, Value ScheduleStart, Value ScheduleFinish, Value EarlyStart, Value EarlyFinish, Value LateStart, Value LateFinish, Value FreeFloat, Value TotalFloat, Value IsCritical, Value StatusTime, Value ActualDuration, Value ActualStart, Value ActualFinish, Value RemainingTime, Value Completion, Value Recurrence) { return ifcapi::express::make_entity("IFC4X1", "IfcTaskTimeRecurring", {{"Name", Name}, {"DataOrigin", DataOrigin}, {"UserDefinedDataOrigin", UserDefinedDataOrigin}, {"DurationType", DurationType}, {"ScheduleDuration", ScheduleDuration}, {"ScheduleStart", ScheduleStart}, {"ScheduleFinish", ScheduleFinish}, {"EarlyStart", EarlyStart}, {"EarlyFinish", EarlyFinish}, {"LateStart", LateStart}, {"LateFinish", LateFinish}, {"FreeFloat", FreeFloat}, {"TotalFloat", TotalFloat}, {"IsCritical", IsCritical}, {"StatusTime", StatusTime}, {"ActualDuration", ActualDuration}, {"ActualStart", ActualStart}, {"ActualFinish", ActualFinish}, {"RemainingTime", RemainingTime}, {"Completion", Completion}, {"Recurrence", Recurrence}}); }
inline Value IfcTaskType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value Identification, Value LongDescription, Value ProcessType, Value PredefinedType, Value WorkMethod) { return ifcapi::express::make_entity("IFC4X1", "IfcTaskType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"Identification", Identification}, {"LongDescription", LongDescription}, {"ProcessType", ProcessType}, {"PredefinedType", PredefinedType}, {"WorkMethod", WorkMethod}}); }
inline Value IfcTelecomAddress(Value Purpose, Value Description, Value UserDefinedPurpose, Value TelephoneNumbers, Value FacsimileNumbers, Value PagerNumber, Value ElectronicMailAddresses, Value WWWHomePageURL, Value MessagingIDs) { return ifcapi::express::make_entity("IFC4X1", "IfcTelecomAddress", {{"Purpose", Purpose}, {"Description", Description}, {"UserDefinedPurpose", UserDefinedPurpose}, {"TelephoneNumbers", TelephoneNumbers}, {"FacsimileNumbers", FacsimileNumbers}, {"PagerNumber", PagerNumber}, {"ElectronicMailAddresses", ElectronicMailAddresses}, {"WWWHomePageURL", WWWHomePageURL}, {"MessagingIDs", MessagingIDs}}); }
inline Value IfcTendon(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value SteelGrade, Value PredefinedType, Value NominalDiameter, Value CrossSectionArea, Value TensionForce, Value PreStress, Value FrictionCoefficient, Value AnchorageSlip, Value MinCurvatureRadius) { return ifcapi::express::make_entity("IFC4X1", "IfcTendon", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"SteelGrade", SteelGrade}, {"PredefinedType", PredefinedType}, {"NominalDiameter", NominalDiameter}, {"CrossSectionArea", CrossSectionArea}, {"TensionForce", TensionForce}, {"PreStress", PreStress}, {"FrictionCoefficient", FrictionCoefficient}, {"AnchorageSlip", AnchorageSlip}, {"MinCurvatureRadius", MinCurvatureRadius}}); }
inline Value IfcTendonAnchor(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value SteelGrade, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcTendonAnchor", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"SteelGrade", SteelGrade}, {"PredefinedType", PredefinedType}}); }
inline Value IfcTendonAnchorType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcTendonAnchorType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcTendonType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType, Value NominalDiameter, Value CrossSectionArea, Value SheathDiameter) { return ifcapi::express::make_entity("IFC4X1", "IfcTendonType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}, {"NominalDiameter", NominalDiameter}, {"CrossSectionArea", CrossSectionArea}, {"SheathDiameter", SheathDiameter}}); }
inline Value IfcTessellatedFaceSet(Value Coordinates) { return ifcapi::express::make_entity("IFC4X1", "IfcTessellatedFaceSet", {{"Coordinates", Coordinates}}); }
inline Value IfcTessellatedItem() { return ifcapi::express::make_entity("IFC4X1", "IfcTessellatedItem", {}); }
inline Value IfcTextLiteral(Value Literal, Value Placement, Value Path) { return ifcapi::express::make_entity("IFC4X1", "IfcTextLiteral", {{"Literal", Literal}, {"Placement", Placement}, {"Path", Path}}); }
inline Value IfcTextLiteralWithExtent(Value Literal, Value Placement, Value Path, Value Extent, Value BoxAlignment) { return ifcapi::express::make_entity("IFC4X1", "IfcTextLiteralWithExtent", {{"Literal", Literal}, {"Placement", Placement}, {"Path", Path}, {"Extent", Extent}, {"BoxAlignment", BoxAlignment}}); }
inline Value IfcTextStyle(Value Name, Value TextCharacterAppearance, Value TextStyle, Value TextFontStyle, Value ModelOrDraughting) { return ifcapi::express::make_entity("IFC4X1", "IfcTextStyle", {{"Name", Name}, {"TextCharacterAppearance", TextCharacterAppearance}, {"TextStyle", TextStyle}, {"TextFontStyle", TextFontStyle}, {"ModelOrDraughting", ModelOrDraughting}}); }
inline Value IfcTextStyleFontModel(Value Name, Value FontFamily, Value FontStyle, Value FontVariant, Value FontWeight, Value FontSize) { return ifcapi::express::make_entity("IFC4X1", "IfcTextStyleFontModel", {{"Name", Name}, {"FontFamily", FontFamily}, {"FontStyle", FontStyle}, {"FontVariant", FontVariant}, {"FontWeight", FontWeight}, {"FontSize", FontSize}}); }
inline Value IfcTextStyleForDefinedFont(Value Colour, Value BackgroundColour) { return ifcapi::express::make_entity("IFC4X1", "IfcTextStyleForDefinedFont", {{"Colour", Colour}, {"BackgroundColour", BackgroundColour}}); }
inline Value IfcTextStyleTextModel(Value TextIndent, Value TextAlign, Value TextDecoration, Value LetterSpacing, Value WordSpacing, Value TextTransform, Value LineHeight) { return ifcapi::express::make_entity("IFC4X1", "IfcTextStyleTextModel", {{"TextIndent", TextIndent}, {"TextAlign", TextAlign}, {"TextDecoration", TextDecoration}, {"LetterSpacing", LetterSpacing}, {"WordSpacing", WordSpacing}, {"TextTransform", TextTransform}, {"LineHeight", LineHeight}}); }
inline Value IfcTextureCoordinate(Value Maps) { return ifcapi::express::make_entity("IFC4X1", "IfcTextureCoordinate", {{"Maps", Maps}}); }
inline Value IfcTextureCoordinateGenerator(Value Maps, Value Mode, Value Parameter) { return ifcapi::express::make_entity("IFC4X1", "IfcTextureCoordinateGenerator", {{"Maps", Maps}, {"Mode", Mode}, {"Parameter", Parameter}}); }
inline Value IfcTextureMap(Value Maps, Value Vertices, Value MappedTo) { return ifcapi::express::make_entity("IFC4X1", "IfcTextureMap", {{"Maps", Maps}, {"Vertices", Vertices}, {"MappedTo", MappedTo}}); }
inline Value IfcTextureVertex(Value Coordinates) { return ifcapi::express::make_entity("IFC4X1", "IfcTextureVertex", {{"Coordinates", Coordinates}}); }
inline Value IfcTextureVertexList(Value TexCoordsList) { return ifcapi::express::make_entity("IFC4X1", "IfcTextureVertexList", {{"TexCoordsList", TexCoordsList}}); }
inline Value IfcTimePeriod(Value StartTime, Value EndTime) { return ifcapi::express::make_entity("IFC4X1", "IfcTimePeriod", {{"StartTime", StartTime}, {"EndTime", EndTime}}); }
inline Value IfcTimeSeries(Value Name, Value Description, Value StartTime, Value EndTime, Value TimeSeriesDataType, Value DataOrigin, Value UserDefinedDataOrigin, Value Unit) { return ifcapi::express::make_entity("IFC4X1", "IfcTimeSeries", {{"Name", Name}, {"Description", Description}, {"StartTime", StartTime}, {"EndTime", EndTime}, {"TimeSeriesDataType", TimeSeriesDataType}, {"DataOrigin", DataOrigin}, {"UserDefinedDataOrigin", UserDefinedDataOrigin}, {"Unit", Unit}}); }
inline Value IfcTimeSeriesValue(Value ListValues) { return ifcapi::express::make_entity("IFC4X1", "IfcTimeSeriesValue", {{"ListValues", ListValues}}); }
inline Value IfcTopologicalRepresentationItem() { return ifcapi::express::make_entity("IFC4X1", "IfcTopologicalRepresentationItem", {}); }
inline Value IfcTopologyRepresentation(Value ContextOfItems, Value RepresentationIdentifier, Value RepresentationType, Value Items) { return ifcapi::express::make_entity("IFC4X1", "IfcTopologyRepresentation", {{"ContextOfItems", ContextOfItems}, {"RepresentationIdentifier", RepresentationIdentifier}, {"RepresentationType", RepresentationType}, {"Items", Items}}); }
inline Value IfcToroidalSurface(Value Position, Value MajorRadius, Value MinorRadius) { return ifcapi::express::make_entity("IFC4X1", "IfcToroidalSurface", {{"Position", Position}, {"MajorRadius", MajorRadius}, {"MinorRadius", MinorRadius}}); }
inline Value IfcTransformer(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcTransformer", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcTransformerType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcTransformerType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcTransitionCurveSegment2D(Value StartPoint, Value StartDirection, Value SegmentLength, Value StartRadius, Value EndRadius, Value IsStartRadiusCCW, Value IsEndRadiusCCW, Value TransitionCurveType) { return ifcapi::express::make_entity("IFC4X1", "IfcTransitionCurveSegment2D", {{"StartPoint", StartPoint}, {"StartDirection", StartDirection}, {"SegmentLength", SegmentLength}, {"StartRadius", StartRadius}, {"EndRadius", EndRadius}, {"IsStartRadiusCCW", IsStartRadiusCCW}, {"IsEndRadiusCCW", IsEndRadiusCCW}, {"TransitionCurveType", TransitionCurveType}}); }
inline Value IfcTransportElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcTransportElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcTransportElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcTransportElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcTrapeziumProfileDef(Value ProfileType, Value ProfileName, Value Position, Value BottomXDim, Value TopXDim, Value YDim, Value TopXOffset) { return ifcapi::express::make_entity("IFC4X1", "IfcTrapeziumProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"BottomXDim", BottomXDim}, {"TopXDim", TopXDim}, {"YDim", YDim}, {"TopXOffset", TopXOffset}}); }
inline Value IfcTriangulatedFaceSet(Value Coordinates, Value Normals, Value Closed, Value CoordIndex, Value PnIndex) { return ifcapi::express::make_entity("IFC4X1", "IfcTriangulatedFaceSet", {{"Coordinates", Coordinates}, {"Normals", Normals}, {"Closed", Closed}, {"CoordIndex", CoordIndex}, {"PnIndex", PnIndex}}); }
inline Value IfcTriangulatedIrregularNetwork(Value Coordinates, Value Normals, Value Closed, Value CoordIndex, Value PnIndex, Value Flags) { return ifcapi::express::make_entity("IFC4X1", "IfcTriangulatedIrregularNetwork", {{"Coordinates", Coordinates}, {"Normals", Normals}, {"Closed", Closed}, {"CoordIndex", CoordIndex}, {"PnIndex", PnIndex}, {"Flags", Flags}}); }
inline Value IfcTrimmedCurve(Value BasisCurve, Value Trim1, Value Trim2, Value SenseAgreement, Value MasterRepresentation) { return ifcapi::express::make_entity("IFC4X1", "IfcTrimmedCurve", {{"BasisCurve", BasisCurve}, {"Trim1", Trim1}, {"Trim2", Trim2}, {"SenseAgreement", SenseAgreement}, {"MasterRepresentation", MasterRepresentation}}); }
inline Value IfcTubeBundle(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcTubeBundle", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcTubeBundleType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcTubeBundleType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcTypeObject(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets) { return ifcapi::express::make_entity("IFC4X1", "IfcTypeObject", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}}); }
inline Value IfcTypeProcess(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value Identification, Value LongDescription, Value ProcessType) { return ifcapi::express::make_entity("IFC4X1", "IfcTypeProcess", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"Identification", Identification}, {"LongDescription", LongDescription}, {"ProcessType", ProcessType}}); }
inline Value IfcTypeProduct(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag) { return ifcapi::express::make_entity("IFC4X1", "IfcTypeProduct", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}}); }
inline Value IfcTypeResource(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value Identification, Value LongDescription, Value ResourceType) { return ifcapi::express::make_entity("IFC4X1", "IfcTypeResource", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"Identification", Identification}, {"LongDescription", LongDescription}, {"ResourceType", ResourceType}}); }
inline Value IfcUShapeProfileDef(Value ProfileType, Value ProfileName, Value Position, Value Depth, Value FlangeWidth, Value WebThickness, Value FlangeThickness, Value FilletRadius, Value EdgeRadius, Value FlangeSlope) { return ifcapi::express::make_entity("IFC4X1", "IfcUShapeProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"Depth", Depth}, {"FlangeWidth", FlangeWidth}, {"WebThickness", WebThickness}, {"FlangeThickness", FlangeThickness}, {"FilletRadius", FilletRadius}, {"EdgeRadius", EdgeRadius}, {"FlangeSlope", FlangeSlope}}); }
inline Value IfcUnitAssignment(Value Units) { return ifcapi::express::make_entity("IFC4X1", "IfcUnitAssignment", {{"Units", Units}}); }
inline Value IfcUnitaryControlElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcUnitaryControlElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcUnitaryControlElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcUnitaryControlElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcUnitaryEquipment(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcUnitaryEquipment", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcUnitaryEquipmentType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcUnitaryEquipmentType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcValve(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcValve", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcValveType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcValveType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcVector(Value Orientation, Value Magnitude) { return ifcapi::express::make_entity("IFC4X1", "IfcVector", {{"Orientation", Orientation}, {"Magnitude", Magnitude}}); }
inline Value IfcVertex() { return ifcapi::express::make_entity("IFC4X1", "IfcVertex", {}); }
inline Value IfcVertexLoop(Value LoopVertex) { return ifcapi::express::make_entity("IFC4X1", "IfcVertexLoop", {{"LoopVertex", LoopVertex}}); }
inline Value IfcVertexPoint(Value VertexGeometry) { return ifcapi::express::make_entity("IFC4X1", "IfcVertexPoint", {{"VertexGeometry", VertexGeometry}}); }
inline Value IfcVibrationIsolator(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcVibrationIsolator", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcVibrationIsolatorType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcVibrationIsolatorType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcVirtualElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC4X1", "IfcVirtualElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcVirtualGridIntersection(Value IntersectingAxes, Value OffsetDistances) { return ifcapi::express::make_entity("IFC4X1", "IfcVirtualGridIntersection", {{"IntersectingAxes", IntersectingAxes}, {"OffsetDistances", OffsetDistances}}); }
inline Value IfcVoidingFeature(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcVoidingFeature", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcWall(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcWall", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcWallElementedCase(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcWallElementedCase", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcWallStandardCase(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcWallStandardCase", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcWallType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcWallType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcWasteTerminal(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcWasteTerminal", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcWasteTerminalType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcWasteTerminalType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcWindow(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value OverallHeight, Value OverallWidth, Value PredefinedType, Value PartitioningType, Value UserDefinedPartitioningType) { return ifcapi::express::make_entity("IFC4X1", "IfcWindow", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"OverallHeight", OverallHeight}, {"OverallWidth", OverallWidth}, {"PredefinedType", PredefinedType}, {"PartitioningType", PartitioningType}, {"UserDefinedPartitioningType", UserDefinedPartitioningType}}); }
inline Value IfcWindowLiningProperties(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value LiningDepth, Value LiningThickness, Value TransomThickness, Value MullionThickness, Value FirstTransomOffset, Value SecondTransomOffset, Value FirstMullionOffset, Value SecondMullionOffset, Value ShapeAspectStyle, Value LiningOffset, Value LiningToPanelOffsetX, Value LiningToPanelOffsetY) { return ifcapi::express::make_entity("IFC4X1", "IfcWindowLiningProperties", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"LiningDepth", LiningDepth}, {"LiningThickness", LiningThickness}, {"TransomThickness", TransomThickness}, {"MullionThickness", MullionThickness}, {"FirstTransomOffset", FirstTransomOffset}, {"SecondTransomOffset", SecondTransomOffset}, {"FirstMullionOffset", FirstMullionOffset}, {"SecondMullionOffset", SecondMullionOffset}, {"ShapeAspectStyle", ShapeAspectStyle}, {"LiningOffset", LiningOffset}, {"LiningToPanelOffsetX", LiningToPanelOffsetX}, {"LiningToPanelOffsetY", LiningToPanelOffsetY}}); }
inline Value IfcWindowPanelProperties(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value OperationType, Value PanelPosition, Value FrameDepth, Value FrameThickness, Value ShapeAspectStyle) { return ifcapi::express::make_entity("IFC4X1", "IfcWindowPanelProperties", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"OperationType", OperationType}, {"PanelPosition", PanelPosition}, {"FrameDepth", FrameDepth}, {"FrameThickness", FrameThickness}, {"ShapeAspectStyle", ShapeAspectStyle}}); }
inline Value IfcWindowStandardCase(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value OverallHeight, Value OverallWidth, Value PredefinedType, Value PartitioningType, Value UserDefinedPartitioningType) { return ifcapi::express::make_entity("IFC4X1", "IfcWindowStandardCase", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"OverallHeight", OverallHeight}, {"OverallWidth", OverallWidth}, {"PredefinedType", PredefinedType}, {"PartitioningType", PartitioningType}, {"UserDefinedPartitioningType", UserDefinedPartitioningType}}); }
inline Value IfcWindowStyle(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ConstructionType, Value OperationType, Value ParameterTakesPrecedence, Value Sizeable) { return ifcapi::express::make_entity("IFC4X1", "IfcWindowStyle", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ConstructionType", ConstructionType}, {"OperationType", OperationType}, {"ParameterTakesPrecedence", ParameterTakesPrecedence}, {"Sizeable", Sizeable}}); }
inline Value IfcWindowType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType, Value PartitioningType, Value ParameterTakesPrecedence, Value UserDefinedPartitioningType) { return ifcapi::express::make_entity("IFC4X1", "IfcWindowType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}, {"PartitioningType", PartitioningType}, {"ParameterTakesPrecedence", ParameterTakesPrecedence}, {"UserDefinedPartitioningType", UserDefinedPartitioningType}}); }
inline Value IfcWorkCalendar(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identification, Value WorkingTimes, Value ExceptionTimes, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcWorkCalendar", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identification", Identification}, {"WorkingTimes", WorkingTimes}, {"ExceptionTimes", ExceptionTimes}, {"PredefinedType", PredefinedType}}); }
inline Value IfcWorkControl(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identification, Value CreationDate, Value Creators, Value Purpose, Value Duration, Value TotalFloat, Value StartTime, Value FinishTime) { return ifcapi::express::make_entity("IFC4X1", "IfcWorkControl", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identification", Identification}, {"CreationDate", CreationDate}, {"Creators", Creators}, {"Purpose", Purpose}, {"Duration", Duration}, {"TotalFloat", TotalFloat}, {"StartTime", StartTime}, {"FinishTime", FinishTime}}); }
inline Value IfcWorkPlan(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identification, Value CreationDate, Value Creators, Value Purpose, Value Duration, Value TotalFloat, Value StartTime, Value FinishTime, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcWorkPlan", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identification", Identification}, {"CreationDate", CreationDate}, {"Creators", Creators}, {"Purpose", Purpose}, {"Duration", Duration}, {"TotalFloat", TotalFloat}, {"StartTime", StartTime}, {"FinishTime", FinishTime}, {"PredefinedType", PredefinedType}}); }
inline Value IfcWorkSchedule(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identification, Value CreationDate, Value Creators, Value Purpose, Value Duration, Value TotalFloat, Value StartTime, Value FinishTime, Value PredefinedType) { return ifcapi::express::make_entity("IFC4X1", "IfcWorkSchedule", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identification", Identification}, {"CreationDate", CreationDate}, {"Creators", Creators}, {"Purpose", Purpose}, {"Duration", Duration}, {"TotalFloat", TotalFloat}, {"StartTime", StartTime}, {"FinishTime", FinishTime}, {"PredefinedType", PredefinedType}}); }
inline Value IfcWorkTime(Value Name, Value DataOrigin, Value UserDefinedDataOrigin, Value RecurrencePattern, Value Start, Value Finish) { return ifcapi::express::make_entity("IFC4X1", "IfcWorkTime", {{"Name", Name}, {"DataOrigin", DataOrigin}, {"UserDefinedDataOrigin", UserDefinedDataOrigin}, {"RecurrencePattern", RecurrencePattern}, {"Start", Start}, {"Finish", Finish}}); }
inline Value IfcZShapeProfileDef(Value ProfileType, Value ProfileName, Value Position, Value Depth, Value FlangeWidth, Value WebThickness, Value FlangeThickness, Value FilletRadius, Value EdgeRadius) { return ifcapi::express::make_entity("IFC4X1", "IfcZShapeProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"Depth", Depth}, {"FlangeWidth", FlangeWidth}, {"WebThickness", WebThickness}, {"FlangeThickness", FlangeThickness}, {"FilletRadius", FilletRadius}, {"EdgeRadius", EdgeRadius}}); }
inline Value IfcZone(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value LongName) { return ifcapi::express::make_entity("IFC4X1", "IfcZone", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"LongName", LongName}}); }

// --- function forward declarations ---
Value IfcAssociatedSurface(Value);
Value IfcBaseAxis(Value, Value, Value, Value);
Value IfcBooleanChoose(Value, Value, Value, Value);
Value IfcBuild2Axes(Value);
Value IfcBuildAxes(Value, Value);
Value IfcConsecutiveSegments(Value);
Value IfcConstraintsParamBSpline(Value, Value, Value, Value, Value);
Value IfcConvertDirectionInto2D(Value);
Value IfcCorrectDimensions(Value, Value);
Value IfcCorrectFillAreaStyle(Value);
Value IfcCorrectLocalPlacement(Value, Value);
Value IfcCorrectObjectAssignment(Value, Value);
Value IfcCorrectUnitAssignment(Value);
Value IfcCrossProduct(Value, Value);
Value IfcCurveDim(Value);
Value IfcCurveWeightsPositive(Value);
Value IfcDeriveDimensionalExponents(Value);
Value IfcDimensionsForSiUnit(Value);
Value IfcDotProduct(Value, Value);
Value IfcFirstProjAxis(Value, Value);
Value IfcGetBasisSurface(Value);
Value IfcListToArray(Value, Value, Value, Value);
Value IfcLoopHeadToTail(Value);
Value IfcMakeArrayOfArray(Value, Value, Value, Value, Value, Value);
Value IfcMlsTotalThickness(Value);
Value IfcNormalise(Value);
Value IfcOrthogonalComplement(Value);
Value IfcPathHeadToTail(Value);
Value IfcPointListDim(Value);
Value IfcSameAxis2Placement(Value, Value, Value);
Value IfcSameCartesianPoint(Value, Value, Value);
Value IfcSameDirection(Value, Value, Value);
Value IfcSameValidPrecision(Value, Value);
Value IfcSameValue(Value, Value, Value);
Value IfcScalarTimesVector(Value, Value);
Value IfcSecondProjAxis(Value, Value, Value);
Value IfcShapeRepresentationTypes(Value, Value);
Value IfcSurfaceWeightsPositive(Value);
Value IfcTaperedSweptAreaProfiles(Value, Value);
Value IfcTopologyRepresentationTypes(Value, Value);
Value IfcUniqueDefinitionNames(Value);
Value IfcUniquePropertyName(Value);
Value IfcUniquePropertySetNames(Value);
Value IfcUniquePropertyTemplateNames(Value);
Value IfcUniqueQuantityNames(Value);
Value IfcVectorDifference(Value, Value);
Value IfcVectorSum(Value, Value);

// --- function definitions ---
Value IfcAssociatedSurface(Value arg) {
    Value surf;
    surf = express_getattr(arg, "BasisSurface");
    return surf;
    return Value();
}

Value IfcBaseAxis(Value dim, Value axis1, Value axis2, Value axis3) {
    Value d1;
    Value d2;
    Value u;
    Value factor;
    if (Value(dim == 3).truthy()) {
        d1 = nvl(IfcNormalise(axis3), IfcDirection(Value::make_list({0.0, 0.0, 1.0})));
        d2 = IfcFirstProjAxis(d1, axis1);
        u = Value::make_list({d2, IfcSecondProjAxis(d1, d2, axis2), d1});
    } else {
        if (Value(exists(axis1)).truthy()) {
            d1 = IfcNormalise(axis1);
            u = Value::make_list({d1, IfcOrthogonalComplement(d1)});
            if (Value(exists(axis2)).truthy()) {
                factor = IfcDotProduct(axis2, express_getitem(u, 2 - 1));
                if (Value(factor < 0.0).truthy()) {
                    u = ifcapi::express::set_index(u, 2 - 1, ifcapi::express::set_attr(ifcapi::express::express_getitem(u, 2 - 1), "DirectionRatios", ifcapi::express::set_index(ifcapi::express::express_getattr(ifcapi::express::express_getitem(u, 2 - 1), "DirectionRatios"), 1 - 1, -express_getitem(express_getattr(express_getitem(u, 2 - 1), "DirectionRatios"), 1 - 1))));
                    u = ifcapi::express::set_index(u, 2 - 1, ifcapi::express::set_attr(ifcapi::express::express_getitem(u, 2 - 1), "DirectionRatios", ifcapi::express::set_index(ifcapi::express::express_getattr(ifcapi::express::express_getitem(u, 2 - 1), "DirectionRatios"), 2 - 1, -express_getitem(express_getattr(express_getitem(u, 2 - 1), "DirectionRatios"), 2 - 1))));
                }
            }
        } else {
            if (Value(exists(axis2)).truthy()) {
                d1 = IfcNormalise(axis2);
                u = Value::make_list({IfcOrthogonalComplement(d1), d1});
                u = ifcapi::express::set_index(u, 1 - 1, ifcapi::express::set_attr(ifcapi::express::express_getitem(u, 1 - 1), "DirectionRatios", ifcapi::express::set_index(ifcapi::express::express_getattr(ifcapi::express::express_getitem(u, 1 - 1), "DirectionRatios"), 1 - 1, -express_getitem(express_getattr(express_getitem(u, 1 - 1), "DirectionRatios"), 1 - 1))));
                u = ifcapi::express::set_index(u, 1 - 1, ifcapi::express::set_attr(ifcapi::express::express_getitem(u, 1 - 1), "DirectionRatios", ifcapi::express::set_index(ifcapi::express::express_getattr(ifcapi::express::express_getitem(u, 1 - 1), "DirectionRatios"), 2 - 1, -express_getitem(express_getattr(express_getitem(u, 1 - 1), "DirectionRatios"), 2 - 1))));
            } else {
                u = Value::make_list({IfcDirection(Value::make_list({1.0, 0.0})), IfcDirection(Value::make_list({0.0, 1.0}))});
            }
        }
    }
    return u;
    return Value();
}

Value IfcBooleanChoose(Value b, Value choice1, Value choice2) {
    if (Value(b).truthy()) {
        return choice1;
    } else {
        return choice2;
    }
    return Value();
}

Value IfcBuild2Axes(Value refdirection) {
    Value d = nvl(IfcNormalise(refdirection), IfcDirection(Value::make_list({1.0, 0.0})));
    return Value::make_list({d, IfcOrthogonalComplement(d)});
    return Value();
}

Value IfcBuildAxes(Value axis, Value refdirection) {
    Value d1;
    Value d2;
    d1 = nvl(IfcNormalise(axis), IfcDirection(Value::make_list({0.0, 0.0, 1.0})));
    d2 = IfcFirstProjAxis(d1, refdirection);
    return Value::make_list({d2, express_getattr(IfcNormalise(IfcCrossProduct(d1, d2)), "Orientation"), d1});
    return Value();
}

Value IfcConsecutiveSegments(Value segments) {
    Value result = true;
    for (Value i = 1; (Value(i) <= Value(hiindex(segments) - 1)).truthy(); i = i + Value((int64_t)1)) {
        if (Value((express_getitem(express_getitem(segments, i - 1), hiindex(express_getitem(segments, i - 1)) - 1)) != (express_getitem(express_getitem(segments, i + 1 - 1), 1 - 1))).truthy()) {
            result = false;
            break;
        }
    }
    return result;
    return Value();
}

Value IfcConstraintsParamBSpline(Value degree, Value upknots, Value upcp, Value knotmult, Value knots) {
    Value sum;
    Value k;
    Value result = true;
    
    sum = express_getitem(knotmult, 1 - 1);
    for (Value i = 2; (Value(i) <= Value(upknots)).truthy(); i = i + Value((int64_t)1)) {
        sum = sum + (express_getitem(knotmult, i - 1));
    }
    if (Value((degree < 1) || (upknots < 2) || (upcp < degree) || (sum != (degree + upcp + 2))).truthy()) {
        result = false;
        return result;
    }
    k = express_getitem(knotmult, 1 - 1);
    if (Value((k < 1) || (k > (degree + 1))).truthy()) {
        result = false;
        return result;
    }
    for (Value i = 2; (Value(i) <= Value(upknots)).truthy(); i = i + Value((int64_t)1)) {
        if (Value(((express_getitem(knotmult, i - 1)) < 1) || ((express_getitem(knots, i - 1)) <= (express_getitem(knots, i - 1 - 1)))).truthy()) {
            result = false;
            return result;
        }
        k = express_getitem(knotmult, i - 1);
        if (Value((i < upknots) && (k > degree)).truthy()) {
            result = false;
            return result;
        }
        if (Value((i == upknots) && (k > (degree + 1))).truthy()) {
            result = false;
            return result;
        }
    }
    return result;
    return Value();
}

Value IfcConvertDirectionInto2D(Value direction) {
    Value direction2d = IfcDirection(Value::make_list({0., 1.}));
    direction2d = ifcapi::express::set_attr(direction2d, "DirectionRatios", ifcapi::express::set_index(ifcapi::express::express_getattr(direction2d, "DirectionRatios"), 1 - 1, express_getitem(express_getattr(direction, "DirectionRatios"), 1 - 1)));
    direction2d = ifcapi::express::set_attr(direction2d, "DirectionRatios", ifcapi::express::set_index(ifcapi::express::express_getattr(direction2d, "DirectionRatios"), 2 - 1, express_getitem(express_getattr(direction, "DirectionRatios"), 2 - 1)));
    return direction2d;
    return Value();
}

Value IfcCorrectDimensions(Value m, Value dim) {
    if (Value((m) == lengthunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(1, 0, 0, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == massunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 1, 0, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == timeunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 0, 1, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == electriccurrentunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 0, 0, 1, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == thermodynamictemperatureunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 0, 0, 0, 1, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == amountofsubstanceunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 0, 0, 0, 0, 1, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == luminousintensityunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 0, 0, 0, 0, 0, 1))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == planeangleunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 0, 0, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == solidangleunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 0, 0, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == areaunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(2, 0, 0, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == volumeunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(3, 0, 0, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == absorbeddoseunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(2, 0, -2, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == radioactivityunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 0, -1, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == electriccapacitanceunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(-2, -1, 4, 2, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == doseequivalentunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(2, 0, -2, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == electricchargeunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 0, 1, 1, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == electricconductanceunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(-2, -1, 3, 2, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == electricvoltageunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(2, 1, -3, -1, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == electricresistanceunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(2, 1, -3, -2, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == energyunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(2, 1, -2, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == forceunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(1, 1, -2, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == frequencyunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 0, -1, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == inductanceunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(2, 1, -2, -2, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == illuminanceunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(-2, 0, 0, 0, 0, 0, 1))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == luminousfluxunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 0, 0, 0, 0, 0, 1))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == magneticfluxunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(2, 1, -2, -1, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == magneticfluxdensityunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 1, -2, -1, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == powerunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(2, 1, -3, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == pressureunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(-1, 1, -2, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    } else {
        return unknown;
    }
    return Value();
}

Value IfcCorrectFillAreaStyle(Value styles) {
    Value hatching = 0;
    Value tiles = 0;
    Value colour = 0;
    Value external = 0;
    external = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& style : ifcapi::express::iter(styles)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcexternallydefinedhatchstyle")), typeof_(style))).truthy()) __r.append(style); } return __r; })());
    hatching = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& style : ifcapi::express::iter(styles)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcfillareastylehatching")), typeof_(style))).truthy()) __r.append(style); } return __r; })());
    tiles = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& style : ifcapi::express::iter(styles)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcfillareastyletiles")), typeof_(style))).truthy()) __r.append(style); } return __r; })());
    colour = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& style : ifcapi::express::iter(styles)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifccolour")), typeof_(style))).truthy()) __r.append(style); } return __r; })());
    if (Value(external > 1).truthy()) {
        return false;
    }
    if (Value((external == 1) && ((hatching > 0) || (tiles > 0) || (colour > 0))).truthy()) {
        return false;
    }
    if (Value(colour > 1).truthy()) {
        return false;
    }
    if (Value((hatching > 0) && (tiles > 0)).truthy()) {
        return false;
    }
    return true;
    return Value();
}

Value IfcCorrectLocalPlacement(Value axisplacement, Value relplacement) {
    if (Value(exists(relplacement)).truthy()) {
        if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcgridplacement")), typeof_(relplacement))).truthy()) {
            return Value();
        }
        if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifclocalplacement")), typeof_(relplacement))).truthy()) {
            if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcaxis2placement2d")), typeof_(axisplacement))).truthy()) {
                return true;
            }
            if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcaxis2placement3d")), typeof_(axisplacement))).truthy()) {
                if (Value((express_getattr(express_getattr(relplacement, "RelativePlacement"), "Dim")) == 3).truthy()) {
                    return true;
                } else {
                    return false;
                }
            }
        }
        return true;
    }
    return Value();
    return Value();
}

Value IfcCorrectObjectAssignment(Value constraint, Value objects) {
    Value count = 0;
    if (Value(!(exists(constraint))).truthy()) {
        return true;
    }
    if (Value((constraint) == Value(std::string("NOTDEFINED"))).truthy()) {
        return true;
    
    } else if (Value((constraint) == Value(std::string("PRODUCT"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(objects)) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcproduct")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })());
        return count == 0;
    
    } else if (Value((constraint) == Value(std::string("PROCESS"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(objects)) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcprocess")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })());
        return count == 0;
    
    } else if (Value((constraint) == Value(std::string("CONTROL"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(objects)) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifccontrol")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })());
        return count == 0;
    
    } else if (Value((constraint) == Value(std::string("RESOURCE"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(objects)) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcresource")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })());
        return count == 0;
    
    } else if (Value((constraint) == Value(std::string("ACTOR"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(objects)) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcactor")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })());
        return count == 0;
    
    } else if (Value((constraint) == Value(std::string("GROUP"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(objects)) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcgroup")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })());
        return count == 0;
    
    } else if (Value((constraint) == Value(std::string("PROJECT"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(objects)) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcproject")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })());
        return count == 0;
    } else {
        return Value();
    }
    return Value();
}

Value IfcCorrectUnitAssignment(Value units) {
    Value namedunitnumber = 0;
    Value derivedunitnumber = 0;
    Value monetaryunitnumber = 0;
    Value namedunitnames = Value::make_list({});
    Value derivedunitnames = Value::make_list({});
    namedunitnumber = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(units)) { if (Value((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcnamedunit")), typeof_(temp))) && (!(((express_getattr(temp, "UnitType")) == Value(std::string("USERDEFINED")))))).truthy()) __r.append(temp); } return __r; })());
    derivedunitnumber = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(units)) { if (Value((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcderivedunit")), typeof_(temp))) && (!(((express_getattr(temp, "UnitType")) == Value(std::string("USERDEFINED")))))).truthy()) __r.append(temp); } return __r; })());
    monetaryunitnumber = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(units)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcmonetaryunit")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    for (Value i = 1; (Value(i) <= Value(sizeof_(units))).truthy(); i = i + Value((int64_t)1)) {
        if (Value((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcnamedunit")), typeof_(express_getitem(units, i - 1)))) && (!(((express_getattr(express_getitem(units, i - 1), "UnitType")) == Value(std::string("USERDEFINED")))))).truthy()) {
            namedunitnames = namedunitnames + (express_getattr(express_getitem(units, i - 1), "UnitType"));
        }
        if (Value((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcderivedunit")), typeof_(express_getitem(units, i - 1)))) && (!(((express_getattr(express_getitem(units, i - 1), "UnitType")) == Value(std::string("USERDEFINED")))))).truthy()) {
            derivedunitnames = derivedunitnames + (express_getattr(express_getitem(units, i - 1), "UnitType"));
        }
    }
    return (sizeof_(namedunitnames) == namedunitnumber) && (sizeof_(derivedunitnames) == derivedunitnumber) && (monetaryunitnumber <= 1);
    return Value();
}

Value IfcCrossProduct(Value arg1, Value arg2) {
    Value v1;
    Value v2;
    Value res;
    Value mag;
    Value result;
    if (Value((!(exists(arg1)) || ((express_getattr(arg1, "Dim")) == 2)) || (!(exists(arg2)) || ((express_getattr(arg2, "Dim")) == 2))).truthy()) {
        return Value();
    } else {
        v1 = express_getattr(IfcNormalise(arg1), "DirectionRatios");
        v2 = express_getattr(IfcNormalise(arg2), "DirectionRatios");
        res = IfcDirection(Value::make_list({((express_getitem(v1, 2 - 1)) * (express_getitem(v2, 3 - 1))) - ((express_getitem(v1, 3 - 1)) * (express_getitem(v2, 2 - 1))), ((express_getitem(v1, 3 - 1)) * (express_getitem(v2, 1 - 1))) - ((express_getitem(v1, 1 - 1)) * (express_getitem(v2, 3 - 1))), ((express_getitem(v1, 1 - 1)) * (express_getitem(v2, 2 - 1))) - ((express_getitem(v1, 2 - 1)) * (express_getitem(v2, 1 - 1)))}));
        mag = 0.0;
        for (Value i = 1; (Value(i) <= Value(3)).truthy(); i = i + Value((int64_t)1)) {
            mag = mag + ((express_getitem(express_getattr(res, "DirectionRatios"), i - 1)) * (express_getitem(express_getattr(res, "DirectionRatios"), i - 1)));
        }
        if (Value(mag > 0.0).truthy()) {
            result = IfcVector(res,  math_sqrt(mag));
        } else {
            result = IfcVector(arg1,  0.0);
        }
        return result;
    }
    return Value();
}

Value IfcCurveDim(Value curve) {
    if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcline")), typeof_(curve))).truthy()) {
        return express_getattr(express_getattr(curve, "Pnt"), "Dim");
    }
    if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcconic")), typeof_(curve))).truthy()) {
        return express_getattr(express_getattr(curve, "Position"), "Dim");
    }
    if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcpolyline")), typeof_(curve))).truthy()) {
        return express_getattr(express_getitem(express_getattr(curve, "Points"), 1 - 1), "Dim");
    }
    if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifctrimmedcurve")), typeof_(curve))).truthy()) {
        return IfcCurveDim(express_getattr(curve, "BasisCurve"));
    }
    if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifccompositecurve")), typeof_(curve))).truthy()) {
        return express_getattr(express_getitem(express_getattr(curve, "Segments"), 1 - 1), "Dim");
    }
    if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcbsplinecurve")), typeof_(curve))).truthy()) {
        return express_getattr(express_getitem(express_getattr(curve, "ControlPointsList"), 1 - 1), "Dim");
    }
    if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcoffsetcurve2d")), typeof_(curve))).truthy()) {
        return 2;
    }
    if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcoffsetcurve3d")), typeof_(curve))).truthy()) {
        return 3;
    }
    if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcoffsetcurvebydistances")), typeof_(curve))).truthy()) {
        return 3;
    }
    if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifccurvesegment2d")), typeof_(curve))).truthy()) {
        return 2;
    }
    if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcalignmentcurve")), typeof_(curve))).truthy()) {
        return 3;
    }
    if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcpcurve")), typeof_(curve))).truthy()) {
        return 3;
    }
    if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcindexedpolycurve")), typeof_(curve))).truthy()) {
        return express_getattr(express_getattr(curve, "Points"), "Dim");
    }
    return Value();
    return Value();
}

Value IfcCurveWeightsPositive(Value b) {
    Value result = true;
    for (Value i = 0; (Value(i) <= Value(express_getattr(b, "UpperIndexOnControlPoints"))).truthy(); i = i + Value((int64_t)1)) {
        if (Value((express_getitem(express_getattr(b, "Weights"), i - 1)) <= 0.0).truthy()) {
            result = false;
            return result;
        }
    }
    return result;
    return Value();
}

Value IfcDeriveDimensionalExponents(Value unitelements) {
    Value result = IfcDimensionalExponents(0, 0, 0, 0, 0, 0, 0);
    for (Value i = loindex(unitelements); (Value(i) <= Value(hiindex(unitelements))).truthy(); i = i + Value((int64_t)1)) {
        result = ifcapi::express::set_attr(result, "LengthExponent", (express_getattr(result, "LengthExponent")) + ((express_getattr(express_getitem(unitelements, i - 1), "Exponent")) * (express_getattr(express_getattr(express_getattr(express_getitem(unitelements, i - 1), "Unit"), "Dimensions"), "LengthExponent"))));
        result = ifcapi::express::set_attr(result, "MassExponent", (express_getattr(result, "MassExponent")) + ((express_getattr(express_getitem(unitelements, i - 1), "Exponent")) * (express_getattr(express_getattr(express_getattr(express_getitem(unitelements, i - 1), "Unit"), "Dimensions"), "MassExponent"))));
        result = ifcapi::express::set_attr(result, "TimeExponent", (express_getattr(result, "TimeExponent")) + ((express_getattr(express_getitem(unitelements, i - 1), "Exponent")) * (express_getattr(express_getattr(express_getattr(express_getitem(unitelements, i - 1), "Unit"), "Dimensions"), "TimeExponent"))));
        result = ifcapi::express::set_attr(result, "ElectricCurrentExponent", (express_getattr(result, "ElectricCurrentExponent")) + ((express_getattr(express_getitem(unitelements, i - 1), "Exponent")) * (express_getattr(express_getattr(express_getattr(express_getitem(unitelements, i - 1), "Unit"), "Dimensions"), "ElectricCurrentExponent"))));
        result = ifcapi::express::set_attr(result, "ThermodynamicTemperatureExponent", (express_getattr(result, "ThermodynamicTemperatureExponent")) + ((express_getattr(express_getitem(unitelements, i - 1), "Exponent")) * (express_getattr(express_getattr(express_getattr(express_getitem(unitelements, i - 1), "Unit"), "Dimensions"), "ThermodynamicTemperatureExponent"))));
        result = ifcapi::express::set_attr(result, "AmountOfSubstanceExponent", (express_getattr(result, "AmountOfSubstanceExponent")) + ((express_getattr(express_getitem(unitelements, i - 1), "Exponent")) * (express_getattr(express_getattr(express_getattr(express_getitem(unitelements, i - 1), "Unit"), "Dimensions"), "AmountOfSubstanceExponent"))));
        result = ifcapi::express::set_attr(result, "LuminousIntensityExponent", (express_getattr(result, "LuminousIntensityExponent")) + ((express_getattr(express_getitem(unitelements, i - 1), "Exponent")) * (express_getattr(express_getattr(express_getattr(express_getitem(unitelements, i - 1), "Unit"), "Dimensions"), "LuminousIntensityExponent"))));
    }
    return result;
    return Value();
}

Value IfcDimensionsForSiUnit(Value n) {
    if (Value((n) == metre).truthy()) {
        return IfcDimensionalExponents(1, 0, 0, 0, 0, 0, 0);
    
    } else if (Value((n) == square_metre).truthy()) {
        return IfcDimensionalExponents(2, 0, 0, 0, 0, 0, 0);
    
    } else if (Value((n) == cubic_metre).truthy()) {
        return IfcDimensionalExponents(3, 0, 0, 0, 0, 0, 0);
    
    } else if (Value((n) == gram).truthy()) {
        return IfcDimensionalExponents(0, 1, 0, 0, 0, 0, 0);
    
    } else if (Value((n) == second).truthy()) {
        return IfcDimensionalExponents(0, 0, 1, 0, 0, 0, 0);
    
    } else if (Value((n) == ampere).truthy()) {
        return IfcDimensionalExponents(0, 0, 0, 1, 0, 0, 0);
    
    } else if (Value((n) == kelvin).truthy()) {
        return IfcDimensionalExponents(0, 0, 0, 0, 1, 0, 0);
    
    } else if (Value((n) == mole).truthy()) {
        return IfcDimensionalExponents(0, 0, 0, 0, 0, 1, 0);
    
    } else if (Value((n) == candela).truthy()) {
        return IfcDimensionalExponents(0, 0, 0, 0, 0, 0, 1);
    
    } else if (Value((n) == radian).truthy()) {
        return IfcDimensionalExponents(0, 0, 0, 0, 0, 0, 0);
    
    } else if (Value((n) == steradian).truthy()) {
        return IfcDimensionalExponents(0, 0, 0, 0, 0, 0, 0);
    
    } else if (Value((n) == hertz).truthy()) {
        return IfcDimensionalExponents(0, 0, -1, 0, 0, 0, 0);
    
    } else if (Value((n) == newton).truthy()) {
        return IfcDimensionalExponents(1, 1, -2, 0, 0, 0, 0);
    
    } else if (Value((n) == pascal).truthy()) {
        return IfcDimensionalExponents(-1, 1, -2, 0, 0, 0, 0);
    
    } else if (Value((n) == joule).truthy()) {
        return IfcDimensionalExponents(2, 1, -2, 0, 0, 0, 0);
    
    } else if (Value((n) == watt).truthy()) {
        return IfcDimensionalExponents(2, 1, -3, 0, 0, 0, 0);
    
    } else if (Value((n) == coulomb).truthy()) {
        return IfcDimensionalExponents(0, 0, 1, 1, 0, 0, 0);
    
    } else if (Value((n) == volt).truthy()) {
        return IfcDimensionalExponents(2, 1, -3, -1, 0, 0, 0);
    
    } else if (Value((n) == farad).truthy()) {
        return IfcDimensionalExponents(-2, -1, 4, 2, 0, 0, 0);
    
    } else if (Value((n) == ohm).truthy()) {
        return IfcDimensionalExponents(2, 1, -3, -2, 0, 0, 0);
    
    } else if (Value((n) == siemens).truthy()) {
        return IfcDimensionalExponents(-2, -1, 3, 2, 0, 0, 0);
    
    } else if (Value((n) == weber).truthy()) {
        return IfcDimensionalExponents(2, 1, -2, -1, 0, 0, 0);
    
    } else if (Value((n) == tesla).truthy()) {
        return IfcDimensionalExponents(0, 1, -2, -1, 0, 0, 0);
    
    } else if (Value((n) == henry).truthy()) {
        return IfcDimensionalExponents(2, 1, -2, -2, 0, 0, 0);
    
    } else if (Value((n) == degree_celsius).truthy()) {
        return IfcDimensionalExponents(0, 0, 0, 0, 1, 0, 0);
    
    } else if (Value((n) == lumen).truthy()) {
        return IfcDimensionalExponents(0, 0, 0, 0, 0, 0, 1);
    
    } else if (Value((n) == lux).truthy()) {
        return IfcDimensionalExponents(-2, 0, 0, 0, 0, 0, 1);
    
    } else if (Value((n) == becquerel).truthy()) {
        return IfcDimensionalExponents(0, 0, -1, 0, 0, 0, 0);
    
    } else if (Value((n) == gray).truthy()) {
        return IfcDimensionalExponents(2, 0, -2, 0, 0, 0, 0);
    
    } else if (Value((n) == sievert).truthy()) {
        return IfcDimensionalExponents(2, 0, -2, 0, 0, 0, 0);
    } else {
        return IfcDimensionalExponents(0, 0, 0, 0, 0, 0, 0);
    }
    return Value();
}

Value IfcDotProduct(Value arg1, Value arg2) {
    Value scalar;
    Value vec1;
    Value vec2;
    Value ndim;
    if (Value(!(exists(arg1)) || !(exists(arg2))).truthy()) {
        scalar = Value();
    } else {
        if (Value((express_getattr(arg1, "Dim")) != (express_getattr(arg2, "Dim"))).truthy()) {
            scalar = Value();
        } else {
            vec1 = IfcNormalise(arg1);
            vec2 = IfcNormalise(arg2);
            ndim = express_getattr(arg1, "Dim");
            scalar = 0.0;
            for (Value i = 1; (Value(i) <= Value(ndim)).truthy(); i = i + Value((int64_t)1)) {
                scalar = scalar + ((express_getitem(express_getattr(vec1, "DirectionRatios"), i - 1)) * (express_getitem(express_getattr(vec2, "DirectionRatios"), i - 1)));
            }
        }
    }
    return scalar;
    return Value();
}

Value IfcFirstProjAxis(Value zaxis, Value arg) {
    Value z;
    Value v;
    Value xvec;
    Value xaxis;
    if (Value(!(exists(zaxis))).truthy()) {
        return Value();
    } else {
        z = IfcNormalise(zaxis);
        if (Value(!(exists(arg))).truthy()) {
            if (Value((express_getattr(z, "DirectionRatios")) != (Value::make_list({1.0, 0.0, 0.0}))).truthy()) {
                v = IfcDirection(Value::make_list({1.0, 0.0, 0.0}));
            } else {
                v = IfcDirection(Value::make_list({0.0, 1.0, 0.0}));
            }
        } else {
            if (Value((express_getattr(arg, "Dim")) != 3).truthy()) {
                return Value();
            }
            if (Value((express_getattr(IfcCrossProduct(arg, z), "Magnitude")) == 0.0).truthy()) {
                return Value();
            } else {
                v = IfcNormalise(arg);
            }
        }
        xvec = IfcScalarTimesVector(IfcDotProduct(v, z), z);
        xaxis = express_getattr(IfcVectorDifference(v, xvec), "Orientation");
        xaxis = IfcNormalise(xaxis);
    }
    return xaxis;
    return Value();
}

Value IfcGetBasisSurface(Value c) {
    Value surfs;
    Value n;
    surfs = Value::make_list({});
    if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcpcurve")), typeof_(c))).truthy()) {
        surfs = Value::make_list({express_getattr(c, "BasisSurface")});
    } else {
        if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcsurfacecurve")), typeof_(c))).truthy()) {
            n = sizeof_(express_getattr(c, "AssociatedGeometry"));
            for (Value i = 1; (Value(i) <= Value(n)).truthy(); i = i + Value((int64_t)1)) {
                surfs = surfs + (IfcAssociatedSurface(express_getitem(express_getattr(c, "AssociatedGeometry"), i - 1)));
            }
        }
    }
    if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifccompositecurveonsurface")), typeof_(c))).truthy()) {
        n = sizeof_(express_getattr(c, "Segments"));
        surfs = IfcGetBasisSurface(express_getattr(express_getitem(express_getattr(c, "Segments"), 1 - 1), "ParentCurve"));
        if (Value(n > 1).truthy()) {
            for (Value i = 2; (Value(i) <= Value(n)).truthy(); i = i + Value((int64_t)1)) {
                surfs = surfs * (IfcGetBasisSurface(express_getattr(express_getitem(express_getattr(c, "Segments"), 1 - 1), "ParentCurve")));
            }
        }
    }
    return surfs;
    return Value();
}

Value IfcListToArray(Value lis, Value low, Value u) {
    Value n;
    Value res;
    n = sizeof_(lis);
    if (Value(n != (u - low + 1)).truthy()) {
        return Value();
    } else {
        res = ifcapi::express::repeat(express_getitem(lis, 1 - 1), n);
        for (Value i = 2; (Value(i) <= Value(n)).truthy(); i = i + Value((int64_t)1)) {
            res = ifcapi::express::set_index(res, low + i - 1 - 1, express_getitem(lis, i - 1));
        }
        return res;
    }
    return Value();
}

Value IfcLoopHeadToTail(Value aloop) {
    Value n;
    
    Value p = true;
    n = sizeof_(express_getattr(aloop, "EdgeList"));
    for (Value i = 2; (Value(i) <= Value(n)).truthy(); i = i + Value((int64_t)1)) {
        p = p && ((express_getattr(express_getitem(express_getattr(aloop, "EdgeList"), i - 1 - 1), "EdgeEnd")) == (express_getattr(express_getitem(express_getattr(aloop, "EdgeList"), i - 1), "EdgeStart")));
    }
    return p;
    return Value();
}

Value IfcMakeArrayOfArray(Value lis, Value low1, Value u1, Value low2, Value u2) {
    Value res;
    if (Value((u1 - low1 + 1) != sizeof_(lis)).truthy()) {
        return Value();
    }
    if (Value((u2 - low2 + 1) != (sizeof_(express_getitem(lis, 1 - 1)))).truthy()) {
        return Value();
    }
    res = ifcapi::express::repeat(IfcListToArray(express_getitem(lis, 1 - 1), low2, u2), u1 - low1 + 1);
    for (Value i = 2; (Value(i) <= Value(hiindex(lis))).truthy(); i = i + Value((int64_t)1)) {
        if (Value((u2 - low2 + 1) != (sizeof_(express_getitem(lis, i - 1)))).truthy()) {
            return Value();
        }
        res = ifcapi::express::set_index(res, low1 + i - 1 - 1, IfcListToArray(express_getitem(lis, i - 1), low2, u2));
    }
    return res;
    return Value();
}

Value IfcMlsTotalThickness(Value layerset) {
    Value max = express_getattr(express_getitem(express_getattr(layerset, "MaterialLayers"), 1 - 1), "LayerThickness");
    if (Value((sizeof_(express_getattr(layerset, "MaterialLayers"))) > 1).truthy()) {
        for (Value i = 2; (Value(i) <= Value(hiindex(express_getattr(layerset, "MaterialLayers")))).truthy(); i = i + Value((int64_t)1)) {
            max = max + (express_getattr(express_getitem(express_getattr(layerset, "MaterialLayers"), i - 1), "LayerThickness"));
        }
    }
    return max;
    return Value();
}

Value IfcNormalise(Value arg) {
    Value ndim;
    Value mag;
    
    Value v = IfcDirection(Value::make_list({1., 0.}));
    Value vec = IfcVector(IfcDirection(Value::make_list({1., 0.})),  1.);
    
    Value result = v;
    if (Value(!(exists(arg))).truthy()) {
        return Value();
    } else {
        if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcvector")), typeof_(arg))).truthy()) {
            ndim = express_getattr(arg, "Dim");
            v = ifcapi::express::set_attr(v, "DirectionRatios", express_getattr(express_getattr(arg, "Orientation"), "DirectionRatios"));
            vec = ifcapi::express::set_attr(vec, "Magnitude", express_getattr(arg, "Magnitude"));
            vec = ifcapi::express::set_attr(vec, "Orientation", v);
            if (Value((express_getattr(arg, "Magnitude")) == 0.0).truthy()) {
                return Value();
            } else {
                vec = ifcapi::express::set_attr(vec, "Magnitude", 1.0);
            }
        } else {
            ndim = express_getattr(arg, "Dim");
            v = ifcapi::express::set_attr(v, "DirectionRatios", express_getattr(arg, "DirectionRatios"));
        }
        mag = 0.0;
        for (Value i = 1; (Value(i) <= Value(ndim)).truthy(); i = i + Value((int64_t)1)) {
            mag = mag + ((express_getitem(express_getattr(v, "DirectionRatios"), i - 1)) * (express_getitem(express_getattr(v, "DirectionRatios"), i - 1)));
        }
        if (Value(mag > 0.0).truthy()) {
            mag = math_sqrt(mag);
            for (Value i = 1; (Value(i) <= Value(ndim)).truthy(); i = i + Value((int64_t)1)) {
                v = ifcapi::express::set_attr(v, "DirectionRatios", ifcapi::express::set_index(ifcapi::express::express_getattr(v, "DirectionRatios"), i - 1, (express_getitem(express_getattr(v, "DirectionRatios"), i - 1)) / mag));
            }
            if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcvector")), typeof_(arg))).truthy()) {
                vec = ifcapi::express::set_attr(vec, "Orientation", v);
                result = vec;
            } else {
                result = v;
            }
        } else {
            return Value();
        }
    }
    return result;
    return Value();
}

Value IfcOrthogonalComplement(Value vec) {
    Value result;
    if (Value(!(exists(vec)) || ((express_getattr(vec, "Dim")) != 2)).truthy()) {
        return Value();
    } else {
        result = IfcDirection(Value::make_list({-express_getitem(express_getattr(vec, "DirectionRatios"), 2 - 1), express_getitem(express_getattr(vec, "DirectionRatios"), 1 - 1)}));
        return result;
    }
    return Value();
}

Value IfcPathHeadToTail(Value apath) {
    Value n = 0;
    Value p = unknown;
    n = sizeof_(express_getattr(apath, "EdgeList"));
    for (Value i = 2; (Value(i) <= Value(n)).truthy(); i = i + Value((int64_t)1)) {
        p = p && ((express_getattr(express_getitem(express_getattr(apath, "EdgeList"), i - 1 - 1), "EdgeEnd")) == (express_getattr(express_getitem(express_getattr(apath, "EdgeList"), i - 1), "EdgeStart")));
    }
    return p;
    return Value();
}

Value IfcPointListDim(Value pointlist) {
    if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifccartesianpointlist2d")), typeof_(pointlist))).truthy()) {
        return 2;
    }
    if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifccartesianpointlist3d")), typeof_(pointlist))).truthy()) {
        return 3;
    }
    return Value();
    return Value();
}

Value IfcSameAxis2Placement(Value ap1, Value ap2, Value epsilon) {
    return (IfcSameDirection(express_getitem(express_getattr(ap1, "P"), 1 - 1), express_getitem(express_getattr(ap2, "P"), 1 - 1), epsilon)) && (IfcSameDirection(express_getitem(express_getattr(ap1, "P"), 2 - 1), express_getitem(express_getattr(ap2, "P"), 2 - 1), epsilon)) && (IfcSameCartesianPoint(express_getattr(ap1, "Location"), express_getattr(ap1, "Location"), epsilon));
    return Value();
}

Value IfcSameCartesianPoint(Value cp1, Value cp2, Value epsilon) {
    Value cp1x = express_getitem(express_getattr(cp1, "Coordinates"), 1 - 1);
    Value cp1y = express_getitem(express_getattr(cp1, "Coordinates"), 2 - 1);
    Value cp1z = 0;
    Value cp2x = express_getitem(express_getattr(cp2, "Coordinates"), 1 - 1);
    Value cp2y = express_getitem(express_getattr(cp2, "Coordinates"), 2 - 1);
    Value cp2z = 0;
    if (Value((sizeof_(express_getattr(cp1, "Coordinates"))) > 2).truthy()) {
        cp1z = express_getitem(express_getattr(cp1, "Coordinates"), 3 - 1);
    }
    if (Value((sizeof_(express_getattr(cp2, "Coordinates"))) > 2).truthy()) {
        cp2z = express_getitem(express_getattr(cp2, "Coordinates"), 3 - 1);
    }
    return (IfcSameValue(cp1x, cp2x, epsilon)) && (IfcSameValue(cp1y, cp2y, epsilon)) && (IfcSameValue(cp1z, cp2z, epsilon));
    return Value();
}

Value IfcSameDirection(Value dir1, Value dir2, Value epsilon) {
    Value dir1x = express_getitem(express_getattr(dir1, "DirectionRatios"), 1 - 1);
    Value dir1y = express_getitem(express_getattr(dir1, "DirectionRatios"), 2 - 1);
    Value dir1z = 0;
    Value dir2x = express_getitem(express_getattr(dir2, "DirectionRatios"), 1 - 1);
    Value dir2y = express_getitem(express_getattr(dir2, "DirectionRatios"), 2 - 1);
    Value dir2z = 0;
    if (Value((sizeof_(express_getattr(dir1, "DirectionRatios"))) > 2).truthy()) {
        dir1z = express_getitem(express_getattr(dir1, "DirectionRatios"), 3 - 1);
    }
    if (Value((sizeof_(express_getattr(dir2, "DirectionRatios"))) > 2).truthy()) {
        dir2z = express_getitem(express_getattr(dir2, "DirectionRatios"), 3 - 1);
    }
    return (IfcSameValue(dir1x, dir2x, epsilon)) && (IfcSameValue(dir1y, dir2y, epsilon)) && (IfcSameValue(dir1z, dir2z, epsilon));
    return Value();
}

Value IfcSameValidPrecision(Value epsilon1, Value epsilon2) {
    Value valideps1;
    Value valideps2;
    
    Value defaulteps = 0.000001;
    Value derivationofeps = 1.001;
    Value uppereps = 1.0;
    valideps1 = nvl(epsilon1, defaulteps);
    valideps2 = nvl(epsilon2, defaulteps);
    return (0.0 < valideps1) && (valideps1 <= (derivationofeps * valideps2)) && (valideps2 <= (derivationofeps * valideps1)) && (valideps2 < uppereps);
    return Value();
}

Value IfcSameValue(Value value1, Value value2, Value epsilon) {
    Value valideps;
    
    Value defaulteps = 0.000001;
    valideps = nvl(epsilon, defaulteps);
    return ((value1 + valideps) > value2) && (value1 < (value2 + valideps));
    return Value();
}

Value IfcScalarTimesVector(Value scalar, Value vec) {
    Value v;
    Value mag;
    Value result;
    if (Value(!(exists(scalar)) || !(exists(vec))).truthy()) {
        return Value();
    } else {
        if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcvector")), typeof_(vec))).truthy()) {
            v = express_getattr(vec, "Orientation");
            mag = scalar * (express_getattr(vec, "Magnitude"));
        } else {
            v = vec;
            mag = scalar;
        }
        if (Value(mag < 0.0).truthy()) {
            for (Value i = 1; (Value(i) <= Value(sizeof_(express_getattr(v, "DirectionRatios")))).truthy(); i = i + Value((int64_t)1)) {
                v = ifcapi::express::set_attr(v, "DirectionRatios", ifcapi::express::set_index(ifcapi::express::express_getattr(v, "DirectionRatios"), i - 1, -express_getitem(express_getattr(v, "DirectionRatios"), i - 1)));
            }
            mag = -mag;
        }
        result = IfcVector(IfcNormalise(v),  mag);
    }
    return result;
    return Value();
}

Value IfcSecondProjAxis(Value zaxis, Value xaxis, Value arg) {
    Value v;
    Value temp;
    Value yaxis;
    if (Value(!(exists(arg))).truthy()) {
        v = IfcDirection(Value::make_list({0.0, 1.0, 0.0}));
    } else {
        v = arg;
    }
    temp = IfcScalarTimesVector(IfcDotProduct(v, zaxis), zaxis);
    yaxis = IfcVectorDifference(v, temp);
    temp = IfcScalarTimesVector(IfcDotProduct(v, xaxis), xaxis);
    yaxis = IfcVectorDifference(yaxis, temp);
    yaxis = IfcNormalise(yaxis);
    return express_getattr(yaxis, "Orientation");
    return Value();
}

Value IfcShapeRepresentationTypes(Value reptype, Value items) {
    Value count = 0;
    if (Value((reptype) == Value(std::string("point"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcpoint")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("pointcloud"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifccartesianpointlist3d")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("curve"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifccurve")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("curve2d"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value((ifcapi::express::express_in(Value(std::string("ifc4x1.ifccurve")), typeof_(temp))) && ((express_getattr(temp, "Dim")) == 2)).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("curve3d"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value((ifcapi::express::express_in(Value(std::string("ifc4x1.ifccurve")), typeof_(temp))) && ((express_getattr(temp, "Dim")) == 3)).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("surface"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcsurface")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("surface2d"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcsurface")), typeof_(temp))) && ((express_getattr(temp, "Dim")) == 2)).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("surface3d"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcsurface")), typeof_(temp))) && ((express_getattr(temp, "Dim")) == 3)).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("fillarea"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcannotationfillarea")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("text"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifctextliteral")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("advancedsurface"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcbsplinesurface")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("annotation2d"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value((sizeof_(typeof_(temp) * (Value::make_list({Value(std::string("ifc4x1.ifcpoint")), Value(std::string("ifc4x1.ifccurve")), Value(std::string("ifc4x1.ifcgeometriccurveset")), Value(std::string("ifc4x1.ifcannotationfillarea")), Value(std::string("ifc4x1.ifctextliteral"))})))) == 1).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("geometricset"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcgeometricset")), typeof_(temp))) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcpoint")), typeof_(temp))) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifccurve")), typeof_(temp))) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcsurface")), typeof_(temp)))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("geometriccurveset"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcgeometriccurveset")), typeof_(temp))) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcgeometricset")), typeof_(temp))) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcpoint")), typeof_(temp))) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifccurve")), typeof_(temp)))).truthy()) __r.append(temp); } return __r; })());
        for (Value i = 1; (Value(i) <= Value(hiindex(items))).truthy(); i = i + Value((int64_t)1)) {
            if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcgeometricset")), typeof_(express_getitem(items, i - 1)))).truthy()) {
                if (Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(express_getitem(items, i - 1), "Elements"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcsurface")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })())) > 0).truthy()) {
                    count = count - 1;
                }
            }
        }
    
    } else if (Value((reptype) == Value(std::string("tessellation"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifctessellateditem")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("surfaceorsolidmodel"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value((sizeof_((Value::make_list({Value(std::string("ifc4x1.ifctessellateditem")), Value(std::string("ifc4x1.ifcshellbasedsurfacemodel")), Value(std::string("ifc4x1.ifcfacebasedsurfacemodel")), Value(std::string("ifc4x1.ifcsolidmodel"))})) * typeof_(temp))) >= 1).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("surfacemodel"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value((sizeof_((Value::make_list({Value(std::string("ifc4x1.ifctessellateditem")), Value(std::string("ifc4x1.ifcshellbasedsurfacemodel")), Value(std::string("ifc4x1.ifcfacebasedsurfacemodel"))})) * typeof_(temp))) >= 1).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("solidmodel"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcsolidmodel")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("sweptsolid"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(((sizeof_((Value::make_list({Value(std::string("ifc4x1.ifcextrudedareasolid")), Value(std::string("ifc4x1.ifcrevolvedareasolid"))})) * typeof_(temp))) >= 1) && ((sizeof_((Value::make_list({Value(std::string("ifc4x1.ifcextrudedareasolidtapered")), Value(std::string("ifc4x1.ifcrevolvedareasolidtapered"))})) * typeof_(temp))) == 0)).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("advancedsweptsolid"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value((sizeof_((Value::make_list({Value(std::string("ifc4x1.ifcsweptareasolid")), Value(std::string("ifc4x1.ifcsweptdisksolid"))})) * typeof_(temp))) >= 1).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("csg"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value((sizeof_((Value::make_list({Value(std::string("ifc4x1.ifcbooleanresult")), Value(std::string("ifc4x1.ifccsgprimitive3d")), Value(std::string("ifc4x1.ifccsgsolid"))})) * typeof_(temp))) >= 1).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("clipping"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value((sizeof_((Value::make_list({Value(std::string("ifc4x1.ifccsgsolid")), Value(std::string("ifc4x1.ifcbooleanclippingresult"))})) * typeof_(temp))) >= 1).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("brep"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcfacetedbrep")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("advancedbrep"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcmanifoldsolidbrep")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("boundingbox"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcboundingbox")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
        if (Value(sizeof_(items) > 1).truthy()) {
            count = 0;
        }
    
    } else if (Value((reptype) == Value(std::string("sectionedspine"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcsectionedspine")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("lightsource"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifclightsource")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("mappedrepresentation"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcmappeditem")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    } else {
        return Value();
    }
    return count == sizeof_(items);
    return Value();
}

Value IfcSurfaceWeightsPositive(Value b) {
    Value result = true;
    Value weights = express_getattr(b, "Weights");
    for (Value i = 0; (Value(i) <= Value(express_getattr(b, "UUpper"))).truthy(); i = i + Value((int64_t)1)) {
        for (Value j = 0; (Value(j) <= Value(express_getattr(b, "VUpper"))).truthy(); j = j + Value((int64_t)1)) {
            if (Value((express_getitem(express_getitem(weights, i - 1), j - 1)) <= 0.0).truthy()) {
                result = false;
                return result;
            }
        }
    }
    return result;
    return Value();
}

Value IfcTaperedSweptAreaProfiles(Value startarea, Value endarea) {
    Value result = false;
    if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcparameterizedprofiledef")), typeof_(startarea))).truthy()) {
        if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcderivedprofiledef")), typeof_(endarea))).truthy()) {
            result = startarea == (express_getattr(endarea, "ParentProfile"));
        } else {
            result = typeof_(startarea) == typeof_(endarea);
        }
    } else {
        if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcderivedprofiledef")), typeof_(endarea))).truthy()) {
            result = startarea == (express_getattr(endarea, "ParentProfile"));
        } else {
            result = false;
        }
    }
    return result;
    return Value();
}

Value IfcTopologyRepresentationTypes(Value reptype, Value items) {
    Value count = 0;
    if (Value((reptype) == Value(std::string("vertex"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcvertex")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("edge"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcedge")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("path"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcpath")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("face"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcface")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("shell"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcopenshell")), typeof_(temp))) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcclosedshell")), typeof_(temp)))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("undefined"))).truthy()) {
        return true;
    } else {
        return Value();
    }
    return count == sizeof_(items);
    return Value();
}

Value IfcUniqueDefinitionNames(Value relations) {
    Value definition;
    Value definitionset;
    Value result;
    
    
    Value properties = Value::make_list({});
    
    if (Value(sizeof_(relations) == 0).truthy()) {
        return true;
    }
    for (Value i = 1; (Value(i) <= Value(hiindex(relations))).truthy(); i = i + Value((int64_t)1)) {
        definition = express_getattr(express_getitem(relations, i - 1), "RelatingPropertyDefinition");
        if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcpropertysetdefinition")), typeof_(definition))).truthy()) {
            properties = properties + definition;
        } else {
            if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcpropertysetdefinitionset")), typeof_(definition))).truthy()) {
                definitionset = definition;
                for (Value j = 1; (Value(j) <= Value(hiindex(definitionset))).truthy(); j = j + Value((int64_t)1)) {
                    properties = properties + (express_getitem(definitionset, j - 1));
                }
            }
        }
    }
    result = IfcUniquePropertySetNames(properties);
    return result;
    return Value();
}

Value IfcUniquePropertyName(Value properties) {
    Value names = Value::make_list({});
    for (Value i = 1; (Value(i) <= Value(hiindex(properties))).truthy(); i = i + Value((int64_t)1)) {
        names = names + (express_getattr(express_getitem(properties, i - 1), "Name"));
    }
    return sizeof_(names) == sizeof_(properties);
    return Value();
}

Value IfcUniquePropertySetNames(Value properties) {
    Value names = Value::make_list({});
    Value unnamed = 0;
    for (Value i = 1; (Value(i) <= Value(hiindex(properties))).truthy(); i = i + Value((int64_t)1)) {
        if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcpropertyset")), typeof_(express_getitem(properties, i - 1)))).truthy()) {
            names = names + (express_getattr(express_getitem(properties, i - 1), "Name"));
        } else {
            unnamed = unnamed + 1;
        }
    }
    return (sizeof_(names) + unnamed) == sizeof_(properties);
    return Value();
}

Value IfcUniquePropertyTemplateNames(Value properties) {
    Value names = Value::make_list({});
    for (Value i = 1; (Value(i) <= Value(hiindex(properties))).truthy(); i = i + Value((int64_t)1)) {
        names = names + (express_getattr(express_getitem(properties, i - 1), "Name"));
    }
    return sizeof_(names) == sizeof_(properties);
    return Value();
}

Value IfcUniqueQuantityNames(Value properties) {
    Value names = Value::make_list({});
    for (Value i = 1; (Value(i) <= Value(hiindex(properties))).truthy(); i = i + Value((int64_t)1)) {
        names = names + (express_getattr(express_getitem(properties, i - 1), "Name"));
    }
    return sizeof_(names) == sizeof_(properties);
    return Value();
}

Value IfcVectorDifference(Value arg1, Value arg2) {
    Value mag1;
    Value vec1;
    Value mag2;
    Value vec2;
    Value ndim;
    Value mag;
    Value res;
    Value result;
    if (Value((!(exists(arg1)) || !(exists(arg2))) || ((express_getattr(arg1, "Dim")) != (express_getattr(arg2, "Dim")))).truthy()) {
        return Value();
    } else {
        if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcvector")), typeof_(arg1))).truthy()) {
            mag1 = express_getattr(arg1, "Magnitude");
            vec1 = express_getattr(arg1, "Orientation");
        } else {
            mag1 = 1.0;
            vec1 = arg1;
        }
        if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcvector")), typeof_(arg2))).truthy()) {
            mag2 = express_getattr(arg2, "Magnitude");
            vec2 = express_getattr(arg2, "Orientation");
        } else {
            mag2 = 1.0;
            vec2 = arg2;
        }
        vec1 = IfcNormalise(vec1);
        vec2 = IfcNormalise(vec2);
        ndim = sizeof_(express_getattr(vec1, "DirectionRatios"));
        mag = 0.0;
        res = IfcDirection(ifcapi::express::repeat(0.0, ndim));
        for (Value i = 1; (Value(i) <= Value(ndim)).truthy(); i = i + Value((int64_t)1)) {
            res = ifcapi::express::set_attr(res, "DirectionRatios", ifcapi::express::set_index(ifcapi::express::express_getattr(res, "DirectionRatios"), i - 1, (mag1 * (express_getitem(express_getattr(vec1, "DirectionRatios"), i - 1))) - (mag2 * (express_getitem(express_getattr(vec2, "DirectionRatios"), i - 1)))));
            mag = mag + ((express_getitem(express_getattr(res, "DirectionRatios"), i - 1)) * (express_getitem(express_getattr(res, "DirectionRatios"), i - 1)));
        }
        if (Value(mag > 0.0).truthy()) {
            result = IfcVector(res,  math_sqrt(mag));
        } else {
            result = IfcVector(vec1,  0.0);
        }
    }
    return result;
    return Value();
}

Value IfcVectorSum(Value arg1, Value arg2) {
    Value mag1;
    Value vec1;
    Value mag2;
    Value vec2;
    Value ndim;
    Value mag;
    Value res;
    Value result;
    if (Value((!(exists(arg1)) || !(exists(arg2))) || ((express_getattr(arg1, "Dim")) != (express_getattr(arg2, "Dim")))).truthy()) {
        return Value();
    } else {
        if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcvector")), typeof_(arg1))).truthy()) {
            mag1 = express_getattr(arg1, "Magnitude");
            vec1 = express_getattr(arg1, "Orientation");
        } else {
            mag1 = 1.0;
            vec1 = arg1;
        }
        if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcvector")), typeof_(arg2))).truthy()) {
            mag2 = express_getattr(arg2, "Magnitude");
            vec2 = express_getattr(arg2, "Orientation");
        } else {
            mag2 = 1.0;
            vec2 = arg2;
        }
        vec1 = IfcNormalise(vec1);
        vec2 = IfcNormalise(vec2);
        ndim = sizeof_(express_getattr(vec1, "DirectionRatios"));
        mag = 0.0;
        res = IfcDirection(ifcapi::express::repeat(0.0, ndim));
        for (Value i = 1; (Value(i) <= Value(ndim)).truthy(); i = i + Value((int64_t)1)) {
            res = ifcapi::express::set_attr(res, "DirectionRatios", ifcapi::express::set_index(ifcapi::express::express_getattr(res, "DirectionRatios"), i - 1, (mag1 * (express_getitem(express_getattr(vec1, "DirectionRatios"), i - 1))) + (mag2 * (express_getitem(express_getattr(vec2, "DirectionRatios"), i - 1)))));
            mag = mag + ((express_getitem(express_getattr(res, "DirectionRatios"), i - 1)) * (express_getitem(express_getattr(res, "DirectionRatios"), i - 1)));
        }
        if (Value(mag > 0.0).truthy()) {
            result = IfcVector(res,  math_sqrt(mag));
        } else {
            result = IfcVector(vec1,  0.0);
        }
    }
    return result;
    return Value();
}

// --- type / entity / rule definitions ---
bool IfcBoxAlignment_WR1(EntityRef self) {
    if (!Value(ifcapi::express::express_in(self, Value::make_list({Value(std::string("top-left")), Value(std::string("top-middle")), Value(std::string("top-right")), Value(std::string("middle-left")), Value(std::string("center")), Value(std::string("middle-right")), Value(std::string("bottom-left")), Value(std::string("bottom-middle")), Value(std::string("bottom-right"))}))).truthy()) return false;
    return true;
}

bool IfcCardinalPointReference_GreaterThanZero(EntityRef self) {
    if (!Value(self > 0).truthy()) return false;
    return true;
}

bool IfcCompoundPlaneAngleMeasure_MinutesInRange(EntityRef self) {
    if (!Value((math_abs(express_getitem(self, 2 - 1))) < 60).truthy()) return false;
    return true;
}

bool IfcCompoundPlaneAngleMeasure_SecondsInRange(EntityRef self) {
    if (!Value((math_abs(express_getitem(self, 3 - 1))) < 60).truthy()) return false;
    return true;
}

bool IfcCompoundPlaneAngleMeasure_MicrosecondsInRange(EntityRef self) {
    if (!Value((sizeof_(self) == 3) || ((math_abs(express_getitem(self, 4 - 1))) < 1000000)).truthy()) return false;
    return true;
}

bool IfcCompoundPlaneAngleMeasure_ConsistentSign(EntityRef self) {
    if (!Value((((express_getitem(self, 1 - 1)) >= 0) && ((express_getitem(self, 2 - 1)) >= 0) && ((express_getitem(self, 3 - 1)) >= 0) && ((sizeof_(self) == 3) || ((express_getitem(self, 4 - 1)) >= 0))) || (((express_getitem(self, 1 - 1)) <= 0) && ((express_getitem(self, 2 - 1)) <= 0) && ((express_getitem(self, 3 - 1)) <= 0) && ((sizeof_(self) == 3) || ((express_getitem(self, 4 - 1)) <= 0)))).truthy()) return false;
    return true;
}

bool IfcDayInMonthNumber_ValidRange(EntityRef self) {
    if (!Value(1 <= self <= 31).truthy()) return false;
    return true;
}

bool IfcDayInWeekNumber_ValidRange(EntityRef self) {
    if (!Value(1 <= self <= 7).truthy()) return false;
    return true;
}

bool IfcDimensionCount_WR1(EntityRef self) {
    if (!Value(0 < self <= 3).truthy()) return false;
    return true;
}

bool IfcFontStyle_WR1(EntityRef self) {
    if (!Value(ifcapi::express::express_in(self, Value::make_list({Value(std::string("normal")), Value(std::string("italic")), Value(std::string("oblique"))}))).truthy()) return false;
    return true;
}

bool IfcFontVariant_WR1(EntityRef self) {
    if (!Value(ifcapi::express::express_in(self, Value::make_list({Value(std::string("normal")), Value(std::string("small-caps"))}))).truthy()) return false;
    return true;
}

bool IfcFontWeight_WR1(EntityRef self) {
    if (!Value(ifcapi::express::express_in(self, Value::make_list({Value(std::string("normal")), Value(std::string("small-caps")), Value(std::string("100")), Value(std::string("200")), Value(std::string("300")), Value(std::string("400")), Value(std::string("500")), Value(std::string("600")), Value(std::string("700")), Value(std::string("800")), Value(std::string("900"))}))).truthy()) return false;
    return true;
}

bool IfcHeatingValueMeasure_WR1(EntityRef self) {
    if (!Value(self > 0.).truthy()) return false;
    return true;
}

bool IfcMonthInYearNumber_ValidRange(EntityRef self) {
    if (!Value(1 <= self <= 12).truthy()) return false;
    return true;
}

bool IfcNonNegativeLengthMeasure_NotNegative(EntityRef self) {
    if (!Value(self >= 0.).truthy()) return false;
    return true;
}

bool IfcNormalisedRatioMeasure_WR1(EntityRef self) {
    if (!Value(0.0 <= self <= 1.0).truthy()) return false;
    return true;
}

bool IfcPHMeasure_WR21(EntityRef self) {
    if (!Value(0.0 <= self <= 14.0).truthy()) return false;
    return true;
}

bool IfcPositiveInteger_WR1(EntityRef self) {
    if (!Value(self > 0).truthy()) return false;
    return true;
}

bool IfcPositiveLengthMeasure_WR1(EntityRef self) {
    if (!Value(self > 0.).truthy()) return false;
    return true;
}

bool IfcPositivePlaneAngleMeasure_WR1(EntityRef self) {
    if (!Value(self > 0.).truthy()) return false;
    return true;
}

bool IfcPositiveRatioMeasure_WR1(EntityRef self) {
    if (!Value(self > 0.).truthy()) return false;
    return true;
}

bool IfcSpecularRoughness_WR1(EntityRef self) {
    if (!Value(0.0 <= self <= 1.0).truthy()) return false;
    return true;
}

bool IfcTextAlignment_WR1(EntityRef self) {
    if (!Value(ifcapi::express::express_in(self, Value::make_list({Value(std::string("left")), Value(std::string("right")), Value(std::string("center")), Value(std::string("justify"))}))).truthy()) return false;
    return true;
}

bool IfcTextDecoration_WR1(EntityRef self) {
    if (!Value(ifcapi::express::express_in(self, Value::make_list({Value(std::string("none")), Value(std::string("underline")), Value(std::string("overline")), Value(std::string("line-through")), Value(std::string("blink"))}))).truthy()) return false;
    return true;
}

bool IfcTextTransformation_WR1(EntityRef self) {
    if (!Value(ifcapi::express::express_in(self, Value::make_list({Value(std::string("capitalize")), Value(std::string("uppercase")), Value(std::string("lowercase")), Value(std::string("none"))}))).truthy()) return false;
    return true;
}

bool IfcActorRole_WR1(EntityRef self) {
    Value role = express_getattr(self, "Role");
    if (!Value((role != Value(std::string("USERDEFINED"))) || ((role == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "UserDefinedRole"))))).truthy()) return false;
    return true;
}

bool IfcActuator_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcActuator_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcactuatortype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcActuatorType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcAddress_WR1(EntityRef self) {
    Value purpose = express_getattr(self, "Purpose");
    if (!Value(!(exists(purpose)) || ((purpose != Value(std::string("USERDEFINED"))) || ((purpose == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "UserDefinedPurpose")))))).truthy()) return false;
    return true;
}

bool IfcAdvancedBrep_HasAdvancedFaces(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& afs : ifcapi::express::iter(express_getattr(express_getattr(self, "Outer"), "CfsFaces"))) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcadvancedface")), typeof_(afs))))).truthy()) __r.append(afs); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcAdvancedBrepWithVoids_VoidsHaveAdvancedFaces(EntityRef self) {
    Value voids = express_getattr(self, "Voids");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& vsh : ifcapi::express::iter(voids)) { if (Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& afs : ifcapi::express::iter(express_getattr(vsh, "CfsFaces"))) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcadvancedface")), typeof_(afs))))).truthy()) __r.append(afs); } return __r; })())) == 0).truthy()) __r.append(vsh); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcAdvancedFace_ApplicableSurface(EntityRef self) {
    if (!Value((sizeof_((Value::make_list({Value(std::string("ifc4x1.ifcelementarysurface")), Value(std::string("ifc4x1.ifcsweptsurface")), Value(std::string("ifc4x1.ifcbsplinesurface"))})) * (typeof_(express_getattr(self, "FaceSurface"))))) == 1).truthy()) return false;
    return true;
}

bool IfcAdvancedFace_RequiresEdgeCurve(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& elpfbnds : ifcapi::express::iter(([&]() { auto __r = Value::make_list({}); for (auto& bnds : ifcapi::express::iter(express_getattr(self, "Bounds"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcedgeloop")), typeof_(express_getattr(bnds, "Bound")))).truthy()) __r.append(bnds); } return __r; })())) { if (Value(!(((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& oe : ifcapi::express::iter(express_getattr(express_getattr(elpfbnds, "Bound"), "EdgeList"))) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcedgecurve")), typeof_(express_getattr(oe, "EdgeElement")))))).truthy()) __r.append(oe); } return __r; })())) == 0))).truthy()) __r.append(elpfbnds); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcAdvancedFace_ApplicableEdgeCurves(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& elpfbnds : ifcapi::express::iter(([&]() { auto __r = Value::make_list({}); for (auto& bnds : ifcapi::express::iter(express_getattr(self, "Bounds"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcedgeloop")), typeof_(express_getattr(bnds, "Bound")))).truthy()) __r.append(bnds); } return __r; })())) { if (Value(!(((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& oe : ifcapi::express::iter(express_getattr(express_getattr(elpfbnds, "Bound"), "EdgeList"))) { if (Value(!(((sizeof_((Value::make_list({Value(std::string("ifc4x1.ifcline")), Value(std::string("ifc4x1.ifcconic")), Value(std::string("ifc4x1.ifcpolyline")), Value(std::string("ifc4x1.ifcbsplinecurve"))})) * (typeof_(express_getattr(express_getattr(oe, "EdgeElement"), "EdgeGeometry"))))) == 1))).truthy()) __r.append(oe); } return __r; })())) == 0))).truthy()) __r.append(elpfbnds); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcAirTerminal_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcAirTerminal_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcairterminaltype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcAirTerminalBox_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcAirTerminalBox_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcairterminalboxtype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcAirTerminalBoxType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcAirTerminalType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcAirToAirHeatRecovery_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcAirToAirHeatRecovery_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcairtoairheatrecoverytype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcAirToAirHeatRecoveryType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcAlarm_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcAlarm_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcalarmtype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcAlarmType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcApproval_HasIdentifierOrName(EntityRef self) {
    Value identifier = express_getattr(self, "Identifier");
    Value name = express_getattr(self, "Name");
    if (!Value(exists(identifier) || exists(name)).truthy()) return false;
    return true;
}

bool IfcArbitraryClosedProfileDef_WR1(EntityRef self) {
    Value outercurve = express_getattr(self, "OuterCurve");
    if (!Value((express_getattr(outercurve, "Dim")) == 2).truthy()) return false;
    return true;
}

bool IfcArbitraryClosedProfileDef_WR2(EntityRef self) {
    Value outercurve = express_getattr(self, "OuterCurve");
    if (!Value(!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcline")), typeof_(outercurve))))).truthy()) return false;
    return true;
}

bool IfcArbitraryClosedProfileDef_WR3(EntityRef self) {
    Value outercurve = express_getattr(self, "OuterCurve");
    if (!Value(!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcoffsetcurve2d")), typeof_(outercurve))))).truthy()) return false;
    return true;
}

bool IfcArbitraryOpenProfileDef_WR11(EntityRef self) {
    if (!Value((ifcapi::express::express_in(Value(std::string("ifc4x1.ifccenterlineprofiledef")), typeof_(self))) || ((express_getattr(self, "ProfileType")) == Value(std::string("CURVE")))).truthy()) return false;
    return true;
}

bool IfcArbitraryOpenProfileDef_WR12(EntityRef self) {
    Value curve = express_getattr(self, "Curve");
    if (!Value((express_getattr(curve, "Dim")) == 2).truthy()) return false;
    return true;
}

bool IfcArbitraryProfileDefWithVoids_WR1(EntityRef self) {
    if (!Value((express_getattr(self, "ProfileType")) == area).truthy()) return false;
    return true;
}

bool IfcArbitraryProfileDefWithVoids_WR2(EntityRef self) {
    Value innercurves = express_getattr(self, "InnerCurves");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(innercurves)) { if (Value((express_getattr(temp, "Dim")) != 2).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcArbitraryProfileDefWithVoids_WR3(EntityRef self) {
    Value innercurves = express_getattr(self, "InnerCurves");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(innercurves)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcline")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcAsymmetricIShapeProfileDef_ValidFlangeThickness(EntityRef self) {
    Value overalldepth = express_getattr(self, "OverallDepth");
    Value bottomflangethickness = express_getattr(self, "BottomFlangeThickness");
    Value topflangethickness = express_getattr(self, "TopFlangeThickness");
    if (!Value(!(exists(topflangethickness)) || ((bottomflangethickness + topflangethickness) < overalldepth)).truthy()) return false;
    return true;
}

bool IfcAsymmetricIShapeProfileDef_ValidWebThickness(EntityRef self) {
    Value bottomflangewidth = express_getattr(self, "BottomFlangeWidth");
    Value webthickness = express_getattr(self, "WebThickness");
    Value topflangewidth = express_getattr(self, "TopFlangeWidth");
    if (!Value((webthickness < bottomflangewidth) && (webthickness < topflangewidth)).truthy()) return false;
    return true;
}

bool IfcAsymmetricIShapeProfileDef_ValidBottomFilletRadius(EntityRef self) {
    Value bottomflangewidth = express_getattr(self, "BottomFlangeWidth");
    Value webthickness = express_getattr(self, "WebThickness");
    Value bottomflangefilletradius = express_getattr(self, "BottomFlangeFilletRadius");
    if (!Value(!(exists(bottomflangefilletradius)) || (bottomflangefilletradius <= ((bottomflangewidth - webthickness) / 2.))).truthy()) return false;
    return true;
}

bool IfcAsymmetricIShapeProfileDef_ValidTopFilletRadius(EntityRef self) {
    Value webthickness = express_getattr(self, "WebThickness");
    Value topflangewidth = express_getattr(self, "TopFlangeWidth");
    Value topflangefilletradius = express_getattr(self, "TopFlangeFilletRadius");
    if (!Value(!(exists(topflangefilletradius)) || (topflangefilletradius <= ((topflangewidth - webthickness) / 2.))).truthy()) return false;
    return true;
}

bool IfcAudioVisualAppliance_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcAudioVisualAppliance_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcaudiovisualappliancetype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcAudioVisualApplianceType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcAxis1Placement_AxisIs3D(EntityRef self) {
    Value axis = express_getattr(self, "Axis");
    if (!Value(!(exists(axis)) || ((express_getattr(axis, "Dim")) == 3)).truthy()) return false;
    return true;
}

bool IfcAxis1Placement_LocationIs3D(EntityRef self) {
    if (!Value((express_getattr(express_getattr(self, "Location"), "Dim")) == 3).truthy()) return false;
    return true;
}

Value calc_IfcAxis1Placement_Z(EntityRef self) {
    Value axis = express_getattr(self, "Axis");
    return nvl(IfcNormalise(axis), IfcDirection(Value::make_list({0.0, 0.0, 1.0})));
}

bool IfcAxis2Placement2D_RefDirIs2D(EntityRef self) {
    Value refdirection = express_getattr(self, "RefDirection");
    if (!Value(!(exists(refdirection)) || ((express_getattr(refdirection, "Dim")) == 2)).truthy()) return false;
    return true;
}

bool IfcAxis2Placement2D_LocationIs2D(EntityRef self) {
    if (!Value((express_getattr(express_getattr(self, "Location"), "Dim")) == 2).truthy()) return false;
    return true;
}

Value calc_IfcAxis2Placement2D_P(EntityRef self) {
    Value refdirection = express_getattr(self, "RefDirection");
    return IfcBuild2Axes(refdirection);
}

bool IfcAxis2Placement3D_LocationIs3D(EntityRef self) {
    if (!Value((express_getattr(express_getattr(self, "Location"), "Dim")) == 3).truthy()) return false;
    return true;
}

bool IfcAxis2Placement3D_AxisIs3D(EntityRef self) {
    Value axis = express_getattr(self, "Axis");
    if (!Value(!(exists(axis)) || ((express_getattr(axis, "Dim")) == 3)).truthy()) return false;
    return true;
}

bool IfcAxis2Placement3D_RefDirIs3D(EntityRef self) {
    Value refdirection = express_getattr(self, "RefDirection");
    if (!Value(!(exists(refdirection)) || ((express_getattr(refdirection, "Dim")) == 3)).truthy()) return false;
    return true;
}

bool IfcAxis2Placement3D_AxisToRefDirPosition(EntityRef self) {
    Value axis = express_getattr(self, "Axis");
    Value refdirection = express_getattr(self, "RefDirection");
    if (!Value(!(exists(axis)) || !(exists(refdirection)) || ((express_getattr(IfcCrossProduct(axis, refdirection), "Magnitude")) > 0.0)).truthy()) return false;
    return true;
}

bool IfcAxis2Placement3D_AxisAndRefDirProvision(EntityRef self) {
    Value axis = express_getattr(self, "Axis");
    Value refdirection = express_getattr(self, "RefDirection");
    if (!Value(!((exists(axis) ^ exists(refdirection)))).truthy()) return false;
    return true;
}

Value calc_IfcAxis2Placement3D_P(EntityRef self) {
    Value axis = express_getattr(self, "Axis");
    Value refdirection = express_getattr(self, "RefDirection");
    return IfcBuildAxes(axis, refdirection);
}

bool IfcBSplineCurve_SameDim(EntityRef self) {
    Value controlpointslist = express_getattr(self, "ControlPointsList");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(controlpointslist)) { if (Value((express_getattr(temp, "Dim")) != (express_getattr(express_getitem(controlpointslist, 1 - 1), "Dim"))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

Value calc_IfcBSplineCurve_UpperIndexOnControlPoints(EntityRef self) {
    Value controlpointslist = express_getattr(self, "ControlPointsList");
    return sizeof_(controlpointslist) - 1;
}

Value calc_IfcBSplineCurve_ControlPoints(EntityRef self) {
    Value controlpointslist = express_getattr(self, "ControlPointsList");
    Value upperindexoncontrolpoints = express_getattr(self, "UpperIndexOnControlPoints");
    return IfcListToArray(controlpointslist, 0, upperindexoncontrolpoints);
}

bool IfcBSplineCurveWithKnots_ConsistentBSpline(EntityRef self) {
    Value degree = express_getattr(self, "Degree");
    Value upperindexoncontrolpoints = express_getattr(self, "UpperIndexOnControlPoints");
    Value knotmultiplicities = express_getattr(self, "KnotMultiplicities");
    Value knots = express_getattr(self, "Knots");
    Value upperindexonknots = express_getattr(self, "UpperIndexOnKnots");
    if (!Value(IfcConstraintsParamBSpline(degree, upperindexonknots, upperindexoncontrolpoints, knotmultiplicities, knots)).truthy()) return false;
    return true;
}

bool IfcBSplineCurveWithKnots_CorrespondingKnotLists(EntityRef self) {
    Value knotmultiplicities = express_getattr(self, "KnotMultiplicities");
    Value upperindexonknots = express_getattr(self, "UpperIndexOnKnots");
    if (!Value(sizeof_(knotmultiplicities) == upperindexonknots).truthy()) return false;
    return true;
}

Value calc_IfcBSplineCurveWithKnots_UpperIndexOnKnots(EntityRef self) {
    Value knots = express_getattr(self, "Knots");
    return sizeof_(knots);
}

Value calc_IfcBSplineSurface_UUpper(EntityRef self) {
    Value controlpointslist = express_getattr(self, "ControlPointsList");
    return sizeof_(controlpointslist) - 1;
}

Value calc_IfcBSplineSurface_VUpper(EntityRef self) {
    Value controlpointslist = express_getattr(self, "ControlPointsList");
    return (sizeof_(express_getitem(controlpointslist, 1 - 1))) - 1;
}

Value calc_IfcBSplineSurface_ControlPoints(EntityRef self) {
    Value controlpointslist = express_getattr(self, "ControlPointsList");
    Value uupper = express_getattr(self, "UUpper");
    Value vupper = express_getattr(self, "VUpper");
    return IfcMakeArrayOfArray(controlpointslist, 0, uupper, 0, vupper);
}

bool IfcBSplineSurfaceWithKnots_UDirectionConstraints(EntityRef self) {
    Value umultiplicities = express_getattr(self, "UMultiplicities");
    Value uknots = express_getattr(self, "UKnots");
    Value knotuupper = express_getattr(self, "KnotUUpper");
    if (!Value(IfcConstraintsParamBSpline(express_getattr(self, "UDegree"), knotuupper, express_getattr(self, "UUpper"), umultiplicities, uknots)).truthy()) return false;
    return true;
}

bool IfcBSplineSurfaceWithKnots_VDirectionConstraints(EntityRef self) {
    Value vmultiplicities = express_getattr(self, "VMultiplicities");
    Value vknots = express_getattr(self, "VKnots");
    Value knotvupper = express_getattr(self, "KnotVUpper");
    if (!Value(IfcConstraintsParamBSpline(express_getattr(self, "VDegree"), knotvupper, express_getattr(self, "VUpper"), vmultiplicities, vknots)).truthy()) return false;
    return true;
}

bool IfcBSplineSurfaceWithKnots_CorrespondingULists(EntityRef self) {
    Value umultiplicities = express_getattr(self, "UMultiplicities");
    Value knotuupper = express_getattr(self, "KnotUUpper");
    if (!Value(sizeof_(umultiplicities) == knotuupper).truthy()) return false;
    return true;
}

bool IfcBSplineSurfaceWithKnots_CorrespondingVLists(EntityRef self) {
    Value vmultiplicities = express_getattr(self, "VMultiplicities");
    Value knotvupper = express_getattr(self, "KnotVUpper");
    if (!Value(sizeof_(vmultiplicities) == knotvupper).truthy()) return false;
    return true;
}

Value calc_IfcBSplineSurfaceWithKnots_KnotVUpper(EntityRef self) {
    Value vknots = express_getattr(self, "VKnots");
    return sizeof_(vknots);
}

Value calc_IfcBSplineSurfaceWithKnots_KnotUUpper(EntityRef self) {
    Value uknots = express_getattr(self, "UKnots");
    return sizeof_(uknots);
}

bool IfcBeam_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcBeam_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcbeamtype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcBeamStandardCase_HasMaterialProfileSetUsage(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(usedin(self, Value(std::string("ifc4x1.ifcrelassociates.relatedobjects"))))) { if (Value((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcrelassociatesmaterial")), typeof_(temp))) && (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcmaterialprofilesetusage")), typeof_(express_getattr(temp, "RelatingMaterial"))))).truthy()) __r.append(temp); } return __r; })())) == 1).truthy()) return false;
    return true;
}

bool IfcBeamType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcBlobTexture_SupportedRasterFormat(EntityRef self) {
    if (!Value(ifcapi::express::express_in(express_getattr(self, "RasterFormat"), Value::make_list({Value(std::string("bmp")), Value(std::string("jpg")), Value(std::string("gif")), Value(std::string("png"))}))).truthy()) return false;
    return true;
}

bool IfcBlobTexture_RasterCodeByteStream(EntityRef self) {
    Value rastercode = express_getattr(self, "RasterCode");
    if (!Value((blength(rastercode) % 8) == 0).truthy()) return false;
    return true;
}

bool IfcBoiler_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcBoiler_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcboilertype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcBoilerType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcBooleanClippingResult_FirstOperandType(EntityRef self) {
    Value firstoperand = express_getattr(self, "FirstOperand");
    if (!Value((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcsweptareasolid")), typeof_(firstoperand))) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcsweptdiscsolid")), typeof_(firstoperand))) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcbooleanclippingresult")), typeof_(firstoperand)))).truthy()) return false;
    return true;
}

bool IfcBooleanClippingResult_SecondOperandType(EntityRef self) {
    Value secondoperand = express_getattr(self, "SecondOperand");
    if (!Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifchalfspacesolid")), typeof_(secondoperand))).truthy()) return false;
    return true;
}

bool IfcBooleanClippingResult_OperatorType(EntityRef self) {
    Value operator_ = express_getattr(self, "Operator");
    if (!Value(operator_ == difference).truthy()) return false;
    return true;
}

bool IfcBooleanResult_SameDim(EntityRef self) {
    Value firstoperand = express_getattr(self, "FirstOperand");
    Value secondoperand = express_getattr(self, "SecondOperand");
    if (!Value((express_getattr(firstoperand, "Dim")) == (express_getattr(secondoperand, "Dim"))).truthy()) return false;
    return true;
}

bool IfcBooleanResult_FirstOperandClosed(EntityRef self) {
    Value firstoperand = express_getattr(self, "FirstOperand");
    if (!Value((!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifctessellatedfaceset")), typeof_(firstoperand))))) || ((exists(express_getattr(firstoperand, "Closed"))) && (express_getattr(firstoperand, "Closed")))).truthy()) return false;
    return true;
}

bool IfcBooleanResult_SecondOperandClosed(EntityRef self) {
    Value secondoperand = express_getattr(self, "SecondOperand");
    if (!Value((!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifctessellatedfaceset")), typeof_(secondoperand))))) || ((exists(express_getattr(secondoperand, "Closed"))) && (express_getattr(secondoperand, "Closed")))).truthy()) return false;
    return true;
}

Value calc_IfcBooleanResult_Dim(EntityRef self) {
    Value firstoperand = express_getattr(self, "FirstOperand");
    return express_getattr(firstoperand, "Dim");
}

bool IfcBoundaryCurve_IsClosed(EntityRef self) {
    if (!Value(express_getattr(self, "ClosedCurve")).truthy()) return false;
    return true;
}

Value calc_IfcBoundingBox_Dim(EntityRef self) {
    return 3;
}

bool IfcBoxedHalfSpace_UnboundedSurface(EntityRef self) {
    if (!Value(!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifccurveboundedplane")), typeof_(express_getattr(self, "BaseSurface")))))).truthy()) return false;
    return true;
}

bool IfcBuildingElement_MaxOneMaterialAssociation(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "HasAssociations"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcrelassociatesmaterial")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })())) <= 1).truthy()) return false;
    return true;
}

bool IfcBuildingElementPart_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcBuildingElementPart_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcbuildingelementparttype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcBuildingElementPartType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcBuildingElementProxy_HasObjectName(EntityRef self) {
    if (!Value(exists(express_getattr(self, "Name"))).truthy()) return false;
    return true;
}

bool IfcBuildingElementProxy_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcBuildingElementProxy_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcbuildingelementproxytype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcBuildingElementProxyType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcBurner_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcBurner_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcburnertype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcBurnerType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcCShapeProfileDef_ValidGirth(EntityRef self) {
    Value depth = express_getattr(self, "Depth");
    Value girth = express_getattr(self, "Girth");
    if (!Value(girth < (depth / 2.)).truthy()) return false;
    return true;
}

bool IfcCShapeProfileDef_ValidInternalFilletRadius(EntityRef self) {
    Value depth = express_getattr(self, "Depth");
    Value width = express_getattr(self, "Width");
    Value wallthickness = express_getattr(self, "WallThickness");
    Value internalfilletradius = express_getattr(self, "InternalFilletRadius");
    if (!Value(!(exists(internalfilletradius)) || ((internalfilletradius <= ((width / 2.) - wallthickness)) && (internalfilletradius <= ((depth / 2.) - wallthickness)))).truthy()) return false;
    return true;
}

bool IfcCShapeProfileDef_ValidWallThickness(EntityRef self) {
    Value depth = express_getattr(self, "Depth");
    Value width = express_getattr(self, "Width");
    Value wallthickness = express_getattr(self, "WallThickness");
    if (!Value((wallthickness < (width / 2.)) && (wallthickness < (depth / 2.))).truthy()) return false;
    return true;
}

bool IfcCableCarrierFitting_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcCableCarrierFitting_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifccablecarrierfittingtype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcCableCarrierFittingType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcCableCarrierSegment_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcCableCarrierSegment_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifccablecarriersegmenttype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcCableCarrierSegmentType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcCableFitting_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcCableFitting_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifccablefittingtype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcCableFittingType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcCableSegment_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcCableSegment_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifccablesegmenttype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcCableSegmentType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcCartesianPoint_CP2Dor3D(EntityRef self) {
    Value coordinates = express_getattr(self, "Coordinates");
    if (!Value(hiindex(coordinates) >= 2).truthy()) return false;
    return true;
}

Value calc_IfcCartesianPoint_Dim(EntityRef self) {
    Value coordinates = express_getattr(self, "Coordinates");
    return hiindex(coordinates);
}

Value calc_IfcCartesianPointList_Dim(EntityRef self) {
    return IfcPointListDim(self);
}

bool IfcCartesianTransformationOperator_ScaleGreaterZero(EntityRef self) {
    Value scl = express_getattr(self, "Scl");
    if (!Value(scl > 0.0).truthy()) return false;
    return true;
}

Value calc_IfcCartesianTransformationOperator_Scl(EntityRef self) {
    Value scale = express_getattr(self, "Scale");
    return nvl(scale, 1.0);
}

Value calc_IfcCartesianTransformationOperator_Dim(EntityRef self) {
    Value localorigin = express_getattr(self, "LocalOrigin");
    return express_getattr(localorigin, "Dim");
}

bool IfcCartesianTransformationOperator2D_DimEqual2(EntityRef self) {
    if (!Value((express_getattr(self, "Dim")) == 2).truthy()) return false;
    return true;
}

bool IfcCartesianTransformationOperator2D_Axis1Is2D(EntityRef self) {
    if (!Value((!((exists(express_getattr(self, "Axis1"))))) || ((express_getattr(express_getattr(self, "Axis1"), "Dim")) == 2)).truthy()) return false;
    return true;
}

bool IfcCartesianTransformationOperator2D_Axis2Is2D(EntityRef self) {
    if (!Value((!((exists(express_getattr(self, "Axis2"))))) || ((express_getattr(express_getattr(self, "Axis2"), "Dim")) == 2)).truthy()) return false;
    return true;
}

Value calc_IfcCartesianTransformationOperator2D_U(EntityRef self) {
    return IfcBaseAxis(2, express_getattr(self, "Axis1"), express_getattr(self, "Axis2"), Value());
}

bool IfcCartesianTransformationOperator2DnonUniform_Scale2GreaterZero(EntityRef self) {
    Value scl2 = express_getattr(self, "Scl2");
    if (!Value(scl2 > 0.0).truthy()) return false;
    return true;
}

Value calc_IfcCartesianTransformationOperator2DnonUniform_Scl2(EntityRef self) {
    Value scale2 = express_getattr(self, "Scale2");
    return nvl(scale2, express_getattr(self, "Scl"));
}

bool IfcCartesianTransformationOperator3D_DimIs3D(EntityRef self) {
    if (!Value((express_getattr(self, "Dim")) == 3).truthy()) return false;
    return true;
}

bool IfcCartesianTransformationOperator3D_Axis1Is3D(EntityRef self) {
    if (!Value((!((exists(express_getattr(self, "Axis1"))))) || ((express_getattr(express_getattr(self, "Axis1"), "Dim")) == 3)).truthy()) return false;
    return true;
}

bool IfcCartesianTransformationOperator3D_Axis2Is3D(EntityRef self) {
    if (!Value((!((exists(express_getattr(self, "Axis2"))))) || ((express_getattr(express_getattr(self, "Axis2"), "Dim")) == 3)).truthy()) return false;
    return true;
}

bool IfcCartesianTransformationOperator3D_Axis3Is3D(EntityRef self) {
    Value axis3 = express_getattr(self, "Axis3");
    if (!Value(!(exists(axis3)) || ((express_getattr(axis3, "Dim")) == 3)).truthy()) return false;
    return true;
}

Value calc_IfcCartesianTransformationOperator3D_U(EntityRef self) {
    Value axis3 = express_getattr(self, "Axis3");
    return IfcBaseAxis(3, express_getattr(self, "Axis1"), express_getattr(self, "Axis2"), axis3);
}

bool IfcCartesianTransformationOperator3DnonUniform_Scale2GreaterZero(EntityRef self) {
    Value scl2 = express_getattr(self, "Scl2");
    if (!Value(scl2 > 0.0).truthy()) return false;
    return true;
}

bool IfcCartesianTransformationOperator3DnonUniform_Scale3GreaterZero(EntityRef self) {
    Value scl3 = express_getattr(self, "Scl3");
    if (!Value(scl3 > 0.0).truthy()) return false;
    return true;
}

Value calc_IfcCartesianTransformationOperator3DnonUniform_Scl2(EntityRef self) {
    Value scale2 = express_getattr(self, "Scale2");
    return nvl(scale2, express_getattr(self, "Scl"));
}

Value calc_IfcCartesianTransformationOperator3DnonUniform_Scl3(EntityRef self) {
    Value scale3 = express_getattr(self, "Scale3");
    return nvl(scale3, express_getattr(self, "Scl"));
}

bool IfcChiller_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcChiller_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcchillertype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcChillerType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcChimney_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcChimney_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcchimneytype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcChimneyType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcCircleHollowProfileDef_WR1(EntityRef self) {
    Value wallthickness = express_getattr(self, "WallThickness");
    if (!Value(wallthickness < (express_getattr(self, "Radius"))).truthy()) return false;
    return true;
}

bool IfcCoil_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcCoil_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifccoiltype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcCoilType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcColumn_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcColumn_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifccolumntype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcColumnStandardCase_HasMaterialProfileSetUsage(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(usedin(self, Value(std::string("ifc4x1.ifcrelassociates.relatedobjects"))))) { if (Value((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcrelassociatesmaterial")), typeof_(temp))) && (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcmaterialprofilesetusage")), typeof_(express_getattr(temp, "RelatingMaterial"))))).truthy()) __r.append(temp); } return __r; })())) == 1).truthy()) return false;
    return true;
}

bool IfcColumnType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcCommunicationsAppliance_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcCommunicationsAppliance_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifccommunicationsappliancetype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcCommunicationsApplianceType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcComplexProperty_WR21(EntityRef self) {
    Value hasproperties = express_getattr(self, "HasProperties");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(hasproperties)) { if (Value(self == temp).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcComplexProperty_WR22(EntityRef self) {
    Value hasproperties = express_getattr(self, "HasProperties");
    if (!Value(IfcUniquePropertyName(hasproperties)).truthy()) return false;
    return true;
}

bool IfcComplexPropertyTemplate_UniquePropertyNames(EntityRef self) {
    Value haspropertytemplates = express_getattr(self, "HasPropertyTemplates");
    if (!Value(IfcUniquePropertyTemplateNames(haspropertytemplates)).truthy()) return false;
    return true;
}

bool IfcComplexPropertyTemplate_NoSelfReference(EntityRef self) {
    Value haspropertytemplates = express_getattr(self, "HasPropertyTemplates");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(haspropertytemplates)) { if (Value(self == temp).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcCompositeCurve_CurveContinuous(EntityRef self) {
    Value segments = express_getattr(self, "Segments");
    Value closedcurve = express_getattr(self, "ClosedCurve");
    if (!Value((!(closedcurve) && ((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(segments)) { if (Value((express_getattr(temp, "Transition")) == discontinuous).truthy()) __r.append(temp); } return __r; })())) == 1)) || (closedcurve && ((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(segments)) { if (Value((express_getattr(temp, "Transition")) == discontinuous).truthy()) __r.append(temp); } return __r; })())) == 0))).truthy()) return false;
    return true;
}

bool IfcCompositeCurve_SameDim(EntityRef self) {
    Value segments = express_getattr(self, "Segments");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(segments)) { if (Value((express_getattr(temp, "Dim")) != (express_getattr(express_getitem(segments, 1 - 1), "Dim"))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

Value calc_IfcCompositeCurve_NSegments(EntityRef self) {
    Value segments = express_getattr(self, "Segments");
    return sizeof_(segments);
}

Value calc_IfcCompositeCurve_ClosedCurve(EntityRef self) {
    Value segments = express_getattr(self, "Segments");
    Value nsegments = express_getattr(self, "NSegments");
    return (express_getattr(express_getitem(segments, nsegments - 1), "Transition")) != discontinuous;
}

bool IfcCompositeCurveOnSurface_SameSurface(EntityRef self) {
    Value basissurface = express_getattr(self, "BasisSurface");
    if (!Value(sizeof_(basissurface) > 0).truthy()) return false;
    return true;
}

Value calc_IfcCompositeCurveOnSurface_BasisSurface(EntityRef self) {
    return IfcGetBasisSurface(self);
}

bool IfcCompositeCurveSegment_ParentIsBoundedCurve(EntityRef self) {
    Value parentcurve = express_getattr(self, "ParentCurve");
    if (!Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcboundedcurve")), typeof_(parentcurve))).truthy()) return false;
    return true;
}

Value calc_IfcCompositeCurveSegment_Dim(EntityRef self) {
    Value parentcurve = express_getattr(self, "ParentCurve");
    return express_getattr(parentcurve, "Dim");
}

bool IfcCompositeProfileDef_InvariantProfileType(EntityRef self) {
    Value profiles = express_getattr(self, "Profiles");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(profiles)) { if (Value((express_getattr(temp, "ProfileType")) != (express_getattr(express_getitem(profiles, 1 - 1), "ProfileType"))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcCompositeProfileDef_NoRecursion(EntityRef self) {
    Value profiles = express_getattr(self, "Profiles");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(profiles)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifccompositeprofiledef")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcCompressor_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcCompressor_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifccompressortype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcCompressorType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcCondenser_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcCondenser_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifccondensertype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcCondenserType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcConstraint_WR11(EntityRef self) {
    Value constraintgrade = express_getattr(self, "ConstraintGrade");
    if (!Value((constraintgrade != Value(std::string("USERDEFINED"))) || ((constraintgrade == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "UserDefinedGrade"))))).truthy()) return false;
    return true;
}

bool IfcConstructionEquipmentResource_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcConstructionEquipmentResourceType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ResourceType"))))).truthy()) return false;
    return true;
}

bool IfcConstructionMaterialResource_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcConstructionMaterialResourceType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ResourceType"))))).truthy()) return false;
    return true;
}

bool IfcConstructionProductResource_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcConstructionProductResourceType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ResourceType"))))).truthy()) return false;
    return true;
}

bool IfcController_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcController_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifccontrollertype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcControllerType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcCooledBeam_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcCooledBeam_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifccooledbeamtype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcCooledBeamType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcCoolingTower_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcCoolingTower_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifccoolingtowertype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcCoolingTowerType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcCovering_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcCovering_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifccoveringtype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcCoveringType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcCrewResource_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcCrewResourceType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ResourceType"))))).truthy()) return false;
    return true;
}

Value calc_IfcCsgPrimitive3D_Dim(EntityRef self) {
    return 3;
}

bool IfcCurtainWall_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcCurtainWall_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifccurtainwalltype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcCurtainWallType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

Value calc_IfcCurve_Dim(EntityRef self) {
    return IfcCurveDim(self);
}

bool IfcCurveStyle_MeasureOfWidth(EntityRef self) {
    Value curvewidth = express_getattr(self, "CurveWidth");
    if (!Value(!(exists(curvewidth)) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcpositivelengthmeasure")), typeof_(curvewidth))) || ((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcdescriptivemeasure")), typeof_(curvewidth))) && (curvewidth == Value(std::string("bylayer"))))).truthy()) return false;
    return true;
}

bool IfcCurveStyle_IdentifiableCurveStyle(EntityRef self) {
    Value curvefont = express_getattr(self, "CurveFont");
    Value curvewidth = express_getattr(self, "CurveWidth");
    Value curvecolour = express_getattr(self, "CurveColour");
    if (!Value(exists(curvefont) || exists(curvewidth) || exists(curvecolour)).truthy()) return false;
    return true;
}

bool IfcCurveStyleFontPattern_VisibleLengthGreaterEqualZero(EntityRef self) {
    Value visiblesegmentlength = express_getattr(self, "VisibleSegmentLength");
    if (!Value(visiblesegmentlength >= 0.).truthy()) return false;
    return true;
}

bool IfcDamper_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcDamper_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcdampertype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcDamperType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcDerivedProfileDef_InvariantProfileType(EntityRef self) {
    Value parentprofile = express_getattr(self, "ParentProfile");
    if (!Value((express_getattr(self, "ProfileType")) == (express_getattr(parentprofile, "ProfileType"))).truthy()) return false;
    return true;
}

bool IfcDerivedUnit_WR1(EntityRef self) {
    Value elements = express_getattr(self, "Elements");
    if (!Value((sizeof_(elements) > 1) || ((sizeof_(elements) == 1) && ((express_getattr(express_getitem(elements, 1 - 1), "Exponent")) != 1))).truthy()) return false;
    return true;
}

bool IfcDerivedUnit_WR2(EntityRef self) {
    Value unittype = express_getattr(self, "UnitType");
    if (!Value((unittype != Value(std::string("USERDEFINED"))) || ((unittype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "UserDefinedType"))))).truthy()) return false;
    return true;
}

Value calc_IfcDerivedUnit_Dimensions(EntityRef self) {
    Value elements = express_getattr(self, "Elements");
    return IfcDeriveDimensionalExponents(elements);
}

bool IfcDirection_MagnitudeGreaterZero(EntityRef self) {
    Value directionratios = express_getattr(self, "DirectionRatios");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& tmp : ifcapi::express::iter(directionratios)) { if (Value(tmp != 0.0).truthy()) __r.append(tmp); } return __r; })())) > 0).truthy()) return false;
    return true;
}

Value calc_IfcDirection_Dim(EntityRef self) {
    Value directionratios = express_getattr(self, "DirectionRatios");
    return hiindex(directionratios);
}

bool IfcDiscreteAccessory_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcDiscreteAccessory_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcdiscreteaccessorytype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcDiscreteAccessoryType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcDistributionChamberElement_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcDistributionChamberElement_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcdistributionchamberelementtype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcDistributionChamberElementType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcDocumentReference_WR1(EntityRef self) {
    Value name = express_getattr(self, "Name");
    Value referenceddocument = express_getattr(self, "ReferencedDocument");
    if (!Value(exists(name) ^ exists(referenceddocument)).truthy()) return false;
    return true;
}

bool IfcDoor_CorrectStyleAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcdoortype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcDoorLiningProperties_WR31(EntityRef self) {
    Value liningdepth = express_getattr(self, "LiningDepth");
    Value liningthickness = express_getattr(self, "LiningThickness");
    if (!Value(!((exists(liningdepth) && !(exists(liningthickness))))).truthy()) return false;
    return true;
}

bool IfcDoorLiningProperties_WR32(EntityRef self) {
    Value thresholddepth = express_getattr(self, "ThresholdDepth");
    Value thresholdthickness = express_getattr(self, "ThresholdThickness");
    if (!Value(!((exists(thresholddepth) && !(exists(thresholdthickness))))).truthy()) return false;
    return true;
}

bool IfcDoorLiningProperties_WR33(EntityRef self) {
    Value transomthickness = express_getattr(self, "TransomThickness");
    Value transomoffset = express_getattr(self, "TransomOffset");
    if (!Value((exists(transomoffset) && exists(transomthickness)) ^ (!(exists(transomoffset)) && !(exists(transomthickness)))).truthy()) return false;
    return true;
}

bool IfcDoorLiningProperties_WR34(EntityRef self) {
    Value casingthickness = express_getattr(self, "CasingThickness");
    Value casingdepth = express_getattr(self, "CasingDepth");
    if (!Value((exists(casingdepth) && exists(casingthickness)) ^ (!(exists(casingdepth)) && !(exists(casingthickness)))).truthy()) return false;
    return true;
}

bool IfcDoorLiningProperties_WR35(EntityRef self) {
    if (!Value((exists(express_getitem(express_getattr(self, "DefinesType"), 1 - 1))) && ((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcdoortype")), typeof_(express_getitem(express_getattr(self, "DefinesType"), 1 - 1)))) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcdoorstyle")), typeof_(express_getitem(express_getattr(self, "DefinesType"), 1 - 1)))))).truthy()) return false;
    return true;
}

bool IfcDoorPanelProperties_ApplicableToType(EntityRef self) {
    if (!Value((exists(express_getitem(express_getattr(self, "DefinesType"), 1 - 1))) && ((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcdoortype")), typeof_(express_getitem(express_getattr(self, "DefinesType"), 1 - 1)))) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcdoorstyle")), typeof_(express_getitem(express_getattr(self, "DefinesType"), 1 - 1)))))).truthy()) return false;
    return true;
}

bool IfcDoorType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcDraughtingPreDefinedColour_PreDefinedColourNames(EntityRef self) {
    if (!Value(ifcapi::express::express_in(express_getattr(self, "Name"), Value::make_list({Value(std::string("black")), Value(std::string("red")), Value(std::string("green")), Value(std::string("blue")), Value(std::string("yellow")), Value(std::string("magenta")), Value(std::string("cyan")), Value(std::string("white")), Value(std::string("bylayer"))}))).truthy()) return false;
    return true;
}

bool IfcDraughtingPreDefinedCurveFont_PreDefinedCurveFontNames(EntityRef self) {
    if (!Value(ifcapi::express::express_in(express_getattr(self, "Name"), Value::make_list({Value(std::string("continuous")), Value(std::string("chain")), Value(std::string("chaindoubledash")), Value(std::string("dashed")), Value(std::string("dotted")), Value(std::string("bylayer"))}))).truthy()) return false;
    return true;
}

bool IfcDuctFitting_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcDuctFitting_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcductfittingtype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcDuctFittingType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcDuctSegment_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcDuctSegment_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcductsegmenttype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcDuctSegmentType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcDuctSilencer_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcDuctSilencer_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcductsilencertype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcDuctSilencerType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcEdgeLoop_IsClosed(EntityRef self) {
    Value edgelist = express_getattr(self, "EdgeList");
    Value ne = express_getattr(self, "Ne");
    if (!Value((express_getattr(express_getitem(edgelist, 1 - 1), "EdgeStart")) == (express_getattr(express_getitem(edgelist, ne - 1), "EdgeEnd"))).truthy()) return false;
    return true;
}

bool IfcEdgeLoop_IsContinuous(EntityRef self) {
    if (!Value(IfcLoopHeadToTail(self)).truthy()) return false;
    return true;
}

Value calc_IfcEdgeLoop_Ne(EntityRef self) {
    Value edgelist = express_getattr(self, "EdgeList");
    return sizeof_(edgelist);
}

bool IfcElectricAppliance_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcElectricAppliance_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcelectricappliancetype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcElectricApplianceType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcElectricDistributionBoard_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcElectricDistributionBoard_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcelectricdistributionboardtype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcElectricDistributionBoardType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcElectricFlowStorageDevice_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcElectricFlowStorageDevice_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcelectricflowstoragedevicetype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcElectricFlowStorageDeviceType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcElectricGenerator_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcElectricGenerator_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcelectricgeneratortype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcElectricGeneratorType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcElectricMotor_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcElectricMotor_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcelectricmotortype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcElectricMotorType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcElectricTimeControl_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcElectricTimeControl_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcelectrictimecontroltype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcElectricTimeControlType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcElementAssembly_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcElementAssembly_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcelementassemblytype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcElementAssemblyType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcElementQuantity_UniqueQuantityNames(EntityRef self) {
    Value quantities = express_getattr(self, "Quantities");
    if (!Value(IfcUniqueQuantityNames(quantities)).truthy()) return false;
    return true;
}

bool IfcEngine_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcEngine_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcenginetype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcEngineType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcEvaporativeCooler_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcEvaporativeCooler_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcevaporativecoolertype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcEvaporativeCoolerType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcEvaporator_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcEvaporator_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcevaporatortype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcEvaporatorType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcEvent_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcEvent_CorrectTypeAssigned(EntityRef self) {
    Value eventtriggertype = express_getattr(self, "EventTriggerType");
    Value userdefinedeventtriggertype = express_getattr(self, "UserDefinedEventTriggerType");
    if (!Value(!(exists(eventtriggertype)) || (eventtriggertype != Value(std::string("USERDEFINED"))) || ((eventtriggertype == Value(std::string("USERDEFINED"))) && exists(userdefinedeventtriggertype))).truthy()) return false;
    return true;
}

bool IfcEventType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ProcessType"))))).truthy()) return false;
    return true;
}

bool IfcEventType_CorrectEventTriggerType(EntityRef self) {
    Value eventtriggertype = express_getattr(self, "EventTriggerType");
    Value userdefinedeventtriggertype = express_getattr(self, "UserDefinedEventTriggerType");
    if (!Value((eventtriggertype != Value(std::string("USERDEFINED"))) || ((eventtriggertype == Value(std::string("USERDEFINED"))) && exists(userdefinedeventtriggertype))).truthy()) return false;
    return true;
}

bool IfcExternalReference_WR1(EntityRef self) {
    Value location = express_getattr(self, "Location");
    Value identification = express_getattr(self, "Identification");
    Value name = express_getattr(self, "Name");
    if (!Value(exists(identification) || exists(location) || exists(name)).truthy()) return false;
    return true;
}

bool IfcExtrudedAreaSolid_ValidExtrusionDirection(EntityRef self) {
    if (!Value((IfcDotProduct(IfcDirection(Value::make_list({0.0, 0.0, 1.0})), express_getattr(self, "ExtrudedDirection"))) != 0.0).truthy()) return false;
    return true;
}

bool IfcExtrudedAreaSolidTapered_CorrectProfileAssignment(EntityRef self) {
    if (!Value(IfcTaperedSweptAreaProfiles(express_getattr(self, "SweptArea"), express_getattr(self, "EndSweptArea"))).truthy()) return false;
    return true;
}

bool IfcFace_HasOuterBound(EntityRef self) {
    Value bounds = express_getattr(self, "Bounds");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(bounds)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcfaceouterbound")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })())) <= 1).truthy()) return false;
    return true;
}

Value calc_IfcFaceBasedSurfaceModel_Dim(EntityRef self) {
    return 3;
}

bool IfcFan_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcFan_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcfantype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcFanType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcFastener_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcFastener_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcfastenertype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcFastenerType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcFeatureElementSubtraction_HasNoSubtraction(EntityRef self) {
    if (!Value((sizeof_(express_getattr(self, "HasOpenings"))) == 0).truthy()) return false;
    return true;
}

bool IfcFeatureElementSubtraction_IsNotFilling(EntityRef self) {
    if (!Value((sizeof_(express_getattr(self, "FillsVoids"))) == 0).truthy()) return false;
    return true;
}

bool IfcFillAreaStyle_MaxOneColour(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& style : ifcapi::express::iter(express_getattr(self, "FillStyles"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifccolour")), typeof_(style))).truthy()) __r.append(style); } return __r; })())) <= 1).truthy()) return false;
    return true;
}

bool IfcFillAreaStyle_MaxOneExtHatchStyle(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& style : ifcapi::express::iter(express_getattr(self, "FillStyles"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcexternallydefinedhatchstyle")), typeof_(style))).truthy()) __r.append(style); } return __r; })())) <= 1).truthy()) return false;
    return true;
}

bool IfcFillAreaStyle_ConsistentHatchStyleDef(EntityRef self) {
    if (!Value(IfcCorrectFillAreaStyle(express_getattr(self, "FillStyles"))).truthy()) return false;
    return true;
}

bool IfcFillAreaStyleHatching_PatternStart2D(EntityRef self) {
    Value patternstart = express_getattr(self, "PatternStart");
    if (!Value(!(exists(patternstart)) || ((express_getattr(patternstart, "Dim")) == 2)).truthy()) return false;
    return true;
}

bool IfcFillAreaStyleHatching_RefHatchLine2D(EntityRef self) {
    Value pointofreferencehatchline = express_getattr(self, "PointOfReferenceHatchLine");
    if (!Value(!(exists(pointofreferencehatchline)) || ((express_getattr(pointofreferencehatchline, "Dim")) == 2)).truthy()) return false;
    return true;
}

bool IfcFilter_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcFilter_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcfiltertype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcFilterType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcFireSuppressionTerminal_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcFireSuppressionTerminal_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcfiresuppressionterminaltype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcFireSuppressionTerminalType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcFixedReferenceSweptAreaSolid_DirectrixBounded(EntityRef self) {
    Value directrix = express_getattr(self, "Directrix");
    Value startparam = express_getattr(self, "StartParam");
    Value endparam = express_getattr(self, "EndParam");
    if (!Value((exists(startparam) && exists(endparam)) || ((sizeof_((Value::make_list({Value(std::string("ifc4x1.ifcconic")), Value(std::string("ifc4x1.ifcboundedcurve"))})) * typeof_(directrix))) == 1)).truthy()) return false;
    return true;
}

bool IfcFlowInstrument_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcFlowInstrument_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcflowinstrumenttype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcFlowInstrumentType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcFlowMeter_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcFlowMeter_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcflowmetertype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcFlowMeterType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcFooting_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcFooting_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcfootingtype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcFootingType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcFurniture_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcFurniture_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcfurnituretype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcFurnitureType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcGeographicElement_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcGeographicElement_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcgeographicelementtype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcGeographicElementType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcGeometricCurveSet_NoSurfaces(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "Elements"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcsurface")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcGeometricRepresentationContext_North2D(EntityRef self) {
    Value truenorth = express_getattr(self, "TrueNorth");
    if (!Value(!(exists(truenorth)) || ((hiindex(express_getattr(truenorth, "DirectionRatios"))) == 2)).truthy()) return false;
    return true;
}

bool IfcGeometricRepresentationSubContext_ParentNoSub(EntityRef self) {
    Value parentcontext = express_getattr(self, "ParentContext");
    if (!Value(!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcgeometricrepresentationsubcontext")), typeof_(parentcontext))))).truthy()) return false;
    return true;
}

bool IfcGeometricRepresentationSubContext_UserTargetProvided(EntityRef self) {
    Value targetview = express_getattr(self, "TargetView");
    Value userdefinedtargetview = express_getattr(self, "UserDefinedTargetView");
    if (!Value((targetview != Value(std::string("USERDEFINED"))) || ((targetview == Value(std::string("USERDEFINED"))) && exists(userdefinedtargetview))).truthy()) return false;
    return true;
}

bool IfcGeometricRepresentationSubContext_NoCoordOperation(EntityRef self) {
    if (!Value((sizeof_(express_getattr(self, "HasCoordinateOperation"))) == 0).truthy()) return false;
    return true;
}

Value calc_IfcGeometricRepresentationSubContext_WorldCoordinateSystem(EntityRef self) {
    Value parentcontext = express_getattr(self, "ParentContext");
    return express_getattr(parentcontext, "WorldCoordinateSystem");
}

Value calc_IfcGeometricRepresentationSubContext_CoordinateSpaceDimension(EntityRef self) {
    Value parentcontext = express_getattr(self, "ParentContext");
    return express_getattr(parentcontext, "CoordinateSpaceDimension");
}

Value calc_IfcGeometricRepresentationSubContext_TrueNorth(EntityRef self) {
    Value parentcontext = express_getattr(self, "ParentContext");
    return nvl(express_getattr(parentcontext, "TrueNorth"), IfcConvertDirectionInto2D(express_getitem(express_getattr(express_getattr(self, "WorldCoordinateSystem"), "P"), 2 - 1)));
}

Value calc_IfcGeometricRepresentationSubContext_Precision(EntityRef self) {
    Value parentcontext = express_getattr(self, "ParentContext");
    return nvl(express_getattr(parentcontext, "Precision"), 1);
}

bool IfcGeometricSet_ConsistentDim(EntityRef self) {
    Value elements = express_getattr(self, "Elements");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(elements)) { if (Value((express_getattr(temp, "Dim")) != (express_getattr(express_getitem(elements, 1 - 1), "Dim"))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

Value calc_IfcGeometricSet_Dim(EntityRef self) {
    Value elements = express_getattr(self, "Elements");
    return express_getattr(express_getitem(elements, 1 - 1), "Dim");
}

bool IfcGridAxis_WR1(EntityRef self) {
    Value axiscurve = express_getattr(self, "AxisCurve");
    if (!Value((express_getattr(axiscurve, "Dim")) == 2).truthy()) return false;
    return true;
}

bool IfcGridAxis_WR2(EntityRef self) {
    Value partofw = express_getattr(self, "PartOfW");
    Value partofv = express_getattr(self, "PartOfV");
    Value partofu = express_getattr(self, "PartOfU");
    if (!Value((sizeof_(partofu) == 1) ^ (sizeof_(partofv) == 1) ^ (sizeof_(partofw) == 1)).truthy()) return false;
    return true;
}

Value calc_IfcHalfSpaceSolid_Dim(EntityRef self) {
    return 3;
}

bool IfcHeatExchanger_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcHeatExchanger_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcheatexchangertype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcHeatExchangerType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcHumidifier_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcHumidifier_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifchumidifiertype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcHumidifierType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcIShapeProfileDef_ValidFlangeThickness(EntityRef self) {
    Value overalldepth = express_getattr(self, "OverallDepth");
    Value flangethickness = express_getattr(self, "FlangeThickness");
    if (!Value((2. * flangethickness) < overalldepth).truthy()) return false;
    return true;
}

bool IfcIShapeProfileDef_ValidWebThickness(EntityRef self) {
    Value overallwidth = express_getattr(self, "OverallWidth");
    Value webthickness = express_getattr(self, "WebThickness");
    if (!Value(webthickness < overallwidth).truthy()) return false;
    return true;
}

bool IfcIShapeProfileDef_ValidFilletRadius(EntityRef self) {
    Value overallwidth = express_getattr(self, "OverallWidth");
    Value overalldepth = express_getattr(self, "OverallDepth");
    Value webthickness = express_getattr(self, "WebThickness");
    Value flangethickness = express_getattr(self, "FlangeThickness");
    Value filletradius = express_getattr(self, "FilletRadius");
    if (!Value(!(exists(filletradius)) || ((filletradius <= ((overallwidth - webthickness) / 2.)) && (filletradius <= ((overalldepth - (2. * flangethickness)) / 2.)))).truthy()) return false;
    return true;
}

bool IfcIndexedPolyCurve_Consecutive(EntityRef self) {
    Value segments = express_getattr(self, "Segments");
    if (!Value((sizeof_(segments) == 0) || IfcConsecutiveSegments(segments)).truthy()) return false;
    return true;
}

bool IfcInterceptor_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcInterceptor_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcinterceptortype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcInterceptorType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcIntersectionCurve_TwoPCurves(EntityRef self) {
    if (!Value((sizeof_(express_getattr(self, "AssociatedGeometry"))) == 2).truthy()) return false;
    return true;
}

bool IfcIntersectionCurve_DistinctSurfaces(EntityRef self) {
    if (!Value((IfcAssociatedSurface(express_getitem(express_getattr(self, "AssociatedGeometry"), 1 - 1))) != (IfcAssociatedSurface(express_getitem(express_getattr(self, "AssociatedGeometry"), 2 - 1)))).truthy()) return false;
    return true;
}

bool IfcJunctionBox_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcJunctionBox_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcjunctionboxtype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcJunctionBoxType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcLShapeProfileDef_ValidThickness(EntityRef self) {
    Value depth = express_getattr(self, "Depth");
    Value width = express_getattr(self, "Width");
    Value thickness = express_getattr(self, "Thickness");
    if (!Value((thickness < depth) && (!(exists(width)) || (thickness < width))).truthy()) return false;
    return true;
}

bool IfcLaborResource_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcLaborResourceType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ResourceType"))))).truthy()) return false;
    return true;
}

bool IfcLamp_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcLamp_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifclamptype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcLampType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcLightFixture_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcLightFixture_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifclightfixturetype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcLightFixtureType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcLine_SameDim(EntityRef self) {
    Value pnt = express_getattr(self, "Pnt");
    Value dir = express_getattr(self, "Dir");
    if (!Value((express_getattr(dir, "Dim")) == (express_getattr(pnt, "Dim"))).truthy()) return false;
    return true;
}

bool IfcLocalPlacement_WR21(EntityRef self) {
    Value placementrelto = express_getattr(self, "PlacementRelTo");
    Value relativeplacement = express_getattr(self, "RelativePlacement");
    if (!Value(IfcCorrectLocalPlacement(relativeplacement, placementrelto)).truthy()) return false;
    return true;
}

bool IfcMaterialDefinitionRepresentation_OnlyStyledRepresentations(EntityRef self) {
    Value representations = express_getattr(self, "Representations");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(representations)) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcstyledrepresentation")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcMaterialLayer_NormalizedPriority(EntityRef self) {
    Value priority = express_getattr(self, "Priority");
    if (!Value(!(exists(priority)) || (0 <= priority <= 100)).truthy()) return false;
    return true;
}

Value calc_IfcMaterialLayerSet_TotalThickness(EntityRef self) {
    return IfcMlsTotalThickness(self);
}

bool IfcMaterialProfile_NormalizedPriority(EntityRef self) {
    Value priority = express_getattr(self, "Priority");
    if (!Value(!(exists(priority)) || (0 <= priority <= 100)).truthy()) return false;
    return true;
}

bool IfcMechanicalFastener_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcMechanicalFastener_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcmechanicalfastenertype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcMechanicalFastenerType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcMedicalDevice_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcMedicalDevice_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcmedicaldevicetype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcMedicalDeviceType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcMember_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcMember_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcmembertype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcMemberStandardCase_HasMaterialProfileSetUsage(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(usedin(self, Value(std::string("ifc4x1.ifcrelassociates.relatedobjects"))))) { if (Value((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcrelassociatesmaterial")), typeof_(temp))) && (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcmaterialprofilesetusage")), typeof_(express_getattr(temp, "RelatingMaterial"))))).truthy()) __r.append(temp); } return __r; })())) == 1).truthy()) return false;
    return true;
}

bool IfcMemberType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

Value calc_IfcMirroredProfileDef_Operator(EntityRef self) {
    return IfcCartesianTransformationOperator2D(IfcDirection(Value::make_list({-1., 0.})),  IfcDirection(Value::make_list({0., 1.})),  IfcCartesianPoint(Value::make_list({0., 0.})),  1.);
}

bool IfcMotorConnection_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcMotorConnection_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcmotorconnectiontype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcMotorConnectionType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcNamedUnit_WR1(EntityRef self) {
    if (!Value(IfcCorrectDimensions(express_getattr(self, "UnitType"), express_getattr(self, "Dimensions"))).truthy()) return false;
    return true;
}

bool IfcObject_UniquePropertySetNames(EntityRef self) {
    Value isdefinedby = express_getattr(self, "IsDefinedBy");
    if (!Value((sizeof_(isdefinedby) == 0) || IfcUniqueDefinitionNames(isdefinedby)).truthy()) return false;
    return true;
}

bool IfcObjective_WR21(EntityRef self) {
    Value objectivequalifier = express_getattr(self, "ObjectiveQualifier");
    if (!Value((objectivequalifier != Value(std::string("USERDEFINED"))) || ((objectivequalifier == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "UserDefinedQualifier"))))).truthy()) return false;
    return true;
}

bool IfcOccupant_WR31(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((!((predefinedtype == Value(std::string("USERDEFINED"))))) || (exists(express_getattr(self, "ObjectType")))).truthy()) return false;
    return true;
}

bool IfcOffsetCurve2D_DimIs2D(EntityRef self) {
    Value basiscurve = express_getattr(self, "BasisCurve");
    if (!Value((express_getattr(basiscurve, "Dim")) == 2).truthy()) return false;
    return true;
}

bool IfcOffsetCurve3D_DimIs2D(EntityRef self) {
    Value basiscurve = express_getattr(self, "BasisCurve");
    if (!Value((express_getattr(basiscurve, "Dim")) == 3).truthy()) return false;
    return true;
}

bool IfcOrientedEdge_EdgeElementNotOriented(EntityRef self) {
    Value edgeelement = express_getattr(self, "EdgeElement");
    if (!Value(!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcorientededge")), typeof_(edgeelement))))).truthy()) return false;
    return true;
}

Value calc_IfcOrientedEdge_EdgeStart(EntityRef self) {
    Value edgeelement = express_getattr(self, "EdgeElement");
    Value orientation = express_getattr(self, "Orientation");
    return IfcBooleanChoose(orientation, express_getattr(edgeelement, "EdgeStart"), express_getattr(edgeelement, "EdgeEnd"));
}

Value calc_IfcOrientedEdge_EdgeEnd(EntityRef self) {
    Value edgeelement = express_getattr(self, "EdgeElement");
    Value orientation = express_getattr(self, "Orientation");
    return IfcBooleanChoose(orientation, express_getattr(edgeelement, "EdgeEnd"), express_getattr(edgeelement, "EdgeStart"));
}

bool IfcOutlet_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcOutlet_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcoutlettype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcOutletType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcOwnerHistory_CorrectChangeAction(EntityRef self) {
    Value changeaction = express_getattr(self, "ChangeAction");
    Value lastmodifieddate = express_getattr(self, "LastModifiedDate");
    if (!Value(exists(lastmodifieddate) || (!(exists(lastmodifieddate)) && !(exists(changeaction))) || (!(exists(lastmodifieddate)) && exists(changeaction) && ((changeaction == Value(std::string("NOTDEFINED"))) || (changeaction == Value(std::string("NOCHANGE")))))).truthy()) return false;
    return true;
}

bool IfcPath_IsContinuous(EntityRef self) {
    if (!Value(IfcPathHeadToTail(self)).truthy()) return false;
    return true;
}

bool IfcPcurve_DimIs2D(EntityRef self) {
    Value referencecurve = express_getattr(self, "ReferenceCurve");
    if (!Value((express_getattr(referencecurve, "Dim")) == 2).truthy()) return false;
    return true;
}

bool IfcPerson_IdentifiablePerson(EntityRef self) {
    Value identification = express_getattr(self, "Identification");
    Value familyname = express_getattr(self, "FamilyName");
    Value givenname = express_getattr(self, "GivenName");
    if (!Value(exists(identification) || exists(familyname) || exists(givenname)).truthy()) return false;
    return true;
}

bool IfcPerson_ValidSetOfNames(EntityRef self) {
    Value familyname = express_getattr(self, "FamilyName");
    Value givenname = express_getattr(self, "GivenName");
    Value middlenames = express_getattr(self, "MiddleNames");
    if (!Value(!(exists(middlenames)) || exists(familyname) || exists(givenname)).truthy()) return false;
    return true;
}

bool IfcPhysicalComplexQuantity_NoSelfReference(EntityRef self) {
    Value hasquantities = express_getattr(self, "HasQuantities");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(hasquantities)) { if (Value(self == temp).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcPhysicalComplexQuantity_UniqueQuantityNames(EntityRef self) {
    Value hasquantities = express_getattr(self, "HasQuantities");
    if (!Value(IfcUniqueQuantityNames(hasquantities)).truthy()) return false;
    return true;
}

bool IfcPile_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcPile_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcpiletype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcPileType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcPipeFitting_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcPipeFitting_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcpipefittingtype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcPipeFittingType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcPipeSegment_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcPipeSegment_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcpipesegmenttype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcPipeSegmentType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcPixelTexture_MinPixelInS(EntityRef self) {
    Value width = express_getattr(self, "Width");
    if (!Value(width >= 1).truthy()) return false;
    return true;
}

bool IfcPixelTexture_MinPixelInT(EntityRef self) {
    Value height = express_getattr(self, "Height");
    if (!Value(height >= 1).truthy()) return false;
    return true;
}

bool IfcPixelTexture_NumberOfColours(EntityRef self) {
    Value colourcomponents = express_getattr(self, "ColourComponents");
    if (!Value(1 <= colourcomponents <= 4).truthy()) return false;
    return true;
}

bool IfcPixelTexture_SizeOfPixelList(EntityRef self) {
    Value width = express_getattr(self, "Width");
    Value height = express_getattr(self, "Height");
    Value pixel = express_getattr(self, "Pixel");
    if (!Value(sizeof_(pixel) == (width * height)).truthy()) return false;
    return true;
}

bool IfcPixelTexture_PixelAsByteAndSameLength(EntityRef self) {
    Value pixel = express_getattr(self, "Pixel");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(pixel)) { if (Value(((blength(temp) % 8) == 0) && (blength(temp) == (blength(express_getitem(pixel, 1 - 1))))).truthy()) __r.append(temp); } return __r; })())) == sizeof_(pixel)).truthy()) return false;
    return true;
}

Value calc_IfcPlacement_Dim(EntityRef self) {
    Value location = express_getattr(self, "Location");
    return express_getattr(location, "Dim");
}

bool IfcPlate_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcPlate_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcplatetype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcPlateStandardCase_HasMaterialLayerSetUsage(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(usedin(self, Value(std::string("ifc4x1.ifcrelassociates.relatedobjects"))))) { if (Value((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcrelassociatesmaterial")), typeof_(temp))) && (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcmateriallayersetusage")), typeof_(express_getattr(temp, "RelatingMaterial"))))).truthy()) __r.append(temp); } return __r; })())) == 1).truthy()) return false;
    return true;
}

bool IfcPlateType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

Value calc_IfcPointOnCurve_Dim(EntityRef self) {
    Value basiscurve = express_getattr(self, "BasisCurve");
    return express_getattr(basiscurve, "Dim");
}

Value calc_IfcPointOnSurface_Dim(EntityRef self) {
    Value basissurface = express_getattr(self, "BasisSurface");
    return express_getattr(basissurface, "Dim");
}

bool IfcPolyLoop_AllPointsSameDim(EntityRef self) {
    Value polygon = express_getattr(self, "Polygon");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(polygon)) { if (Value((express_getattr(temp, "Dim")) != (express_getattr(express_getitem(polygon, 1 - 1), "Dim"))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcPolygonalBoundedHalfSpace_BoundaryDim(EntityRef self) {
    Value polygonalboundary = express_getattr(self, "PolygonalBoundary");
    if (!Value((express_getattr(polygonalboundary, "Dim")) == 2).truthy()) return false;
    return true;
}

bool IfcPolygonalBoundedHalfSpace_BoundaryType(EntityRef self) {
    Value polygonalboundary = express_getattr(self, "PolygonalBoundary");
    if (!Value((sizeof_(typeof_(polygonalboundary) * (Value::make_list({Value(std::string("ifc4x1.ifcpolyline")), Value(std::string("ifc4x1.ifccompositecurve"))})))) == 1).truthy()) return false;
    return true;
}

bool IfcPolyline_SameDim(EntityRef self) {
    Value points = express_getattr(self, "Points");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(points)) { if (Value((express_getattr(temp, "Dim")) != (express_getattr(express_getitem(points, 1 - 1), "Dim"))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcPositioningElement_HasPlacement(EntityRef self) {
    if (!Value(exists(express_getattr(self, "ObjectPlacement"))).truthy()) return false;
    return true;
}

bool IfcPostalAddress_WR1(EntityRef self) {
    Value internallocation = express_getattr(self, "InternalLocation");
    Value addresslines = express_getattr(self, "AddressLines");
    Value postalbox = express_getattr(self, "PostalBox");
    Value town = express_getattr(self, "Town");
    Value region = express_getattr(self, "Region");
    Value postalcode = express_getattr(self, "PostalCode");
    Value country = express_getattr(self, "Country");
    if (!Value(exists(internallocation) || exists(addresslines) || exists(postalbox) || exists(postalcode) || exists(town) || exists(region) || exists(country)).truthy()) return false;
    return true;
}

bool IfcPresentationLayerAssignment_ApplicableItems(EntityRef self) {
    Value assigneditems = express_getattr(self, "AssignedItems");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(assigneditems)) { if (Value((sizeof_(typeof_(temp) * (Value::make_list({Value(std::string("ifc4x1.ifcshaperepresentation")), Value(std::string("ifc4x1.ifcgeometricrepresentationitem")), Value(std::string("ifc4x1.ifcmappeditem"))})))) == 1).truthy()) __r.append(temp); } return __r; })())) == sizeof_(assigneditems)).truthy()) return false;
    return true;
}

bool IfcPresentationLayerWithStyle_ApplicableOnlyToItems(EntityRef self) {
    Value assigneditems = express_getattr(self, "AssignedItems");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(assigneditems)) { if (Value((sizeof_(typeof_(temp) * (Value::make_list({Value(std::string("ifc4x1.ifcgeometricrepresentationitem")), Value(std::string("ifc4x1.ifcmappeditem"))})))) >= 1).truthy()) __r.append(temp); } return __r; })())) == sizeof_(assigneditems)).truthy()) return false;
    return true;
}

bool IfcProcedure_HasName(EntityRef self) {
    if (!Value(exists(express_getattr(self, "Name"))).truthy()) return false;
    return true;
}

bool IfcProcedure_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcProcedureType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ProcessType"))))).truthy()) return false;
    return true;
}

bool IfcProduct_PlacementForShapeRepresentation(EntityRef self) {
    Value objectplacement = express_getattr(self, "ObjectPlacement");
    Value representation = express_getattr(self, "Representation");
    if (!Value((exists(representation) && exists(objectplacement)) || (exists(representation) && ((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(representation, "Representations"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcshaperepresentation")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })())) == 0)) || !(exists(representation))).truthy()) return false;
    return true;
}

bool IfcProductDefinitionShape_OnlyShapeModel(EntityRef self) {
    Value representations = express_getattr(self, "Representations");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(representations)) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcshapemodel")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcProject_HasName(EntityRef self) {
    if (!Value(exists(express_getattr(self, "Name"))).truthy()) return false;
    return true;
}

bool IfcProject_CorrectContext(EntityRef self) {
    if (!Value((!((exists(express_getattr(self, "RepresentationContexts"))))) || ((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "RepresentationContexts"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcgeometricrepresentationsubcontext")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })())) == 0)).truthy()) return false;
    return true;
}

bool IfcProject_NoDecomposition(EntityRef self) {
    if (!Value((sizeof_(express_getattr(self, "Decomposes"))) == 0).truthy()) return false;
    return true;
}

bool IfcProjectedCRS_IsLengthUnit(EntityRef self) {
    Value mapunit = express_getattr(self, "MapUnit");
    if (!Value(!(exists(mapunit)) || ((express_getattr(mapunit, "UnitType")) == Value(std::string("LENGTHUNIT")))).truthy()) return false;
    return true;
}

bool IfcPropertyBoundedValue_SameUnitUpperLower(EntityRef self) {
    Value upperboundvalue = express_getattr(self, "UpperBoundValue");
    Value lowerboundvalue = express_getattr(self, "LowerBoundValue");
    if (!Value(!(exists(upperboundvalue)) || !(exists(lowerboundvalue)) || (typeof_(upperboundvalue) == typeof_(lowerboundvalue))).truthy()) return false;
    return true;
}

bool IfcPropertyBoundedValue_SameUnitUpperSet(EntityRef self) {
    Value upperboundvalue = express_getattr(self, "UpperBoundValue");
    Value setpointvalue = express_getattr(self, "SetPointValue");
    if (!Value(!(exists(upperboundvalue)) || !(exists(setpointvalue)) || (typeof_(upperboundvalue) == typeof_(setpointvalue))).truthy()) return false;
    return true;
}

bool IfcPropertyBoundedValue_SameUnitLowerSet(EntityRef self) {
    Value lowerboundvalue = express_getattr(self, "LowerBoundValue");
    Value setpointvalue = express_getattr(self, "SetPointValue");
    if (!Value(!(exists(lowerboundvalue)) || !(exists(setpointvalue)) || (typeof_(lowerboundvalue) == typeof_(setpointvalue))).truthy()) return false;
    return true;
}

bool IfcPropertyDependencyRelationship_NoSelfReference(EntityRef self) {
    Value dependingproperty = express_getattr(self, "DependingProperty");
    Value dependantproperty = express_getattr(self, "DependantProperty");
    if (!Value(dependingproperty != dependantproperty).truthy()) return false;
    return true;
}

bool IfcPropertyEnumeratedValue_WR21(EntityRef self) {
    Value enumerationvalues = express_getattr(self, "EnumerationValues");
    Value enumerationreference = express_getattr(self, "EnumerationReference");
    if (!Value(!(exists(enumerationreference)) || !(exists(enumerationvalues)) || ((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(enumerationvalues)) { if (Value(ifcapi::express::express_in(temp, express_getattr(enumerationreference, "EnumerationValues"))).truthy()) __r.append(temp); } return __r; })())) == sizeof_(enumerationvalues))).truthy()) return false;
    return true;
}

bool IfcPropertyEnumeration_WR01(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "EnumerationValues"))) { if (Value(!(((typeof_(express_getitem(express_getattr(self, "EnumerationValues"), 1 - 1))) == typeof_(temp)))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcPropertyListValue_WR31(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "ListValues"))) { if (Value(!(((typeof_(express_getitem(express_getattr(self, "ListValues"), 1 - 1))) == typeof_(temp)))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcPropertySet_ExistsName(EntityRef self) {
    if (!Value(exists(express_getattr(self, "Name"))).truthy()) return false;
    return true;
}

bool IfcPropertySet_UniquePropertyNames(EntityRef self) {
    Value hasproperties = express_getattr(self, "HasProperties");
    if (!Value(IfcUniquePropertyName(hasproperties)).truthy()) return false;
    return true;
}

bool IfcPropertySetTemplate_ExistsName(EntityRef self) {
    if (!Value(exists(express_getattr(self, "Name"))).truthy()) return false;
    return true;
}

bool IfcPropertySetTemplate_UniquePropertyNames(EntityRef self) {
    Value haspropertytemplates = express_getattr(self, "HasPropertyTemplates");
    if (!Value(IfcUniquePropertyTemplateNames(haspropertytemplates)).truthy()) return false;
    return true;
}

bool IfcPropertyTableValue_WR21(EntityRef self) {
    Value definingvalues = express_getattr(self, "DefiningValues");
    Value definedvalues = express_getattr(self, "DefinedValues");
    if (!Value((!(exists(definingvalues)) && !(exists(definedvalues))) || (sizeof_(definingvalues) == sizeof_(definedvalues))).truthy()) return false;
    return true;
}

bool IfcPropertyTableValue_WR22(EntityRef self) {
    Value definingvalues = express_getattr(self, "DefiningValues");
    if (!Value(!(exists(definingvalues)) || ((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "DefiningValues"))) { if (Value(typeof_(temp) != (typeof_(express_getitem(express_getattr(self, "DefiningValues"), 1 - 1)))).truthy()) __r.append(temp); } return __r; })())) == 0)).truthy()) return false;
    return true;
}

bool IfcPropertyTableValue_WR23(EntityRef self) {
    Value definedvalues = express_getattr(self, "DefinedValues");
    if (!Value(!(exists(definedvalues)) || ((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "DefinedValues"))) { if (Value(typeof_(temp) != (typeof_(express_getitem(express_getattr(self, "DefinedValues"), 1 - 1)))).truthy()) __r.append(temp); } return __r; })())) == 0)).truthy()) return false;
    return true;
}

bool IfcProtectiveDevice_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcProtectiveDevice_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcprotectivedevicetype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcProtectiveDeviceTrippingUnit_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcProtectiveDeviceTrippingUnit_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcprotectivedevicetrippingunittype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcProtectiveDeviceTrippingUnitType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcProtectiveDeviceType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcProxy_WR1(EntityRef self) {
    if (!Value(exists(express_getattr(self, "Name"))).truthy()) return false;
    return true;
}

bool IfcPump_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcPump_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcpumptype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcPumpType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcQuantityArea_WR21(EntityRef self) {
    if (!Value((!((exists(express_getattr(self, "Unit"))))) || ((express_getattr(express_getattr(self, "Unit"), "UnitType")) == Value(std::string("AREAUNIT")))).truthy()) return false;
    return true;
}

bool IfcQuantityArea_WR22(EntityRef self) {
    Value areavalue = express_getattr(self, "AreaValue");
    if (!Value(areavalue >= 0.).truthy()) return false;
    return true;
}

bool IfcQuantityCount_WR21(EntityRef self) {
    Value countvalue = express_getattr(self, "CountValue");
    if (!Value(countvalue >= 0.).truthy()) return false;
    return true;
}

bool IfcQuantityLength_WR21(EntityRef self) {
    if (!Value((!((exists(express_getattr(self, "Unit"))))) || ((express_getattr(express_getattr(self, "Unit"), "UnitType")) == Value(std::string("LENGTHUNIT")))).truthy()) return false;
    return true;
}

bool IfcQuantityLength_WR22(EntityRef self) {
    Value lengthvalue = express_getattr(self, "LengthValue");
    if (!Value(lengthvalue >= 0.).truthy()) return false;
    return true;
}

bool IfcQuantityTime_WR21(EntityRef self) {
    if (!Value((!((exists(express_getattr(self, "Unit"))))) || ((express_getattr(express_getattr(self, "Unit"), "UnitType")) == Value(std::string("TIMEUNIT")))).truthy()) return false;
    return true;
}

bool IfcQuantityTime_WR22(EntityRef self) {
    Value timevalue = express_getattr(self, "TimeValue");
    if (!Value(timevalue >= 0.).truthy()) return false;
    return true;
}

bool IfcQuantityVolume_WR21(EntityRef self) {
    if (!Value((!((exists(express_getattr(self, "Unit"))))) || ((express_getattr(express_getattr(self, "Unit"), "UnitType")) == Value(std::string("VOLUMEUNIT")))).truthy()) return false;
    return true;
}

bool IfcQuantityVolume_WR22(EntityRef self) {
    Value volumevalue = express_getattr(self, "VolumeValue");
    if (!Value(volumevalue >= 0.).truthy()) return false;
    return true;
}

bool IfcQuantityWeight_WR21(EntityRef self) {
    if (!Value((!((exists(express_getattr(self, "Unit"))))) || ((express_getattr(express_getattr(self, "Unit"), "UnitType")) == Value(std::string("MASSUNIT")))).truthy()) return false;
    return true;
}

bool IfcQuantityWeight_WR22(EntityRef self) {
    Value weightvalue = express_getattr(self, "WeightValue");
    if (!Value(weightvalue >= 0.).truthy()) return false;
    return true;
}

bool IfcRailing_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcRailing_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcrailingtype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcRailingType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcRamp_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcRamp_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcramptype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcRampFlight_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcRampFlight_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcrampflighttype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcRampFlightType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcRampType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcRationalBSplineCurveWithKnots_SameNumOfWeightsAndPoints(EntityRef self) {
    Value weightsdata = express_getattr(self, "WeightsData");
    if (!Value(sizeof_(weightsdata) == (sizeof_(express_getattr(self, "ControlPointsList")))).truthy()) return false;
    return true;
}

bool IfcRationalBSplineCurveWithKnots_WeightsGreaterZero(EntityRef self) {
    if (!Value(IfcCurveWeightsPositive(self)).truthy()) return false;
    return true;
}

Value calc_IfcRationalBSplineCurveWithKnots_Weights(EntityRef self) {
    Value weightsdata = express_getattr(self, "WeightsData");
    return IfcListToArray(weightsdata, 0, express_getattr(self, "UpperIndexOnControlPoints"));
}

bool IfcRationalBSplineSurfaceWithKnots_CorrespondingWeightsDataLists(EntityRef self) {
    Value weightsdata = express_getattr(self, "WeightsData");
    if (!Value((sizeof_(weightsdata) == (sizeof_(express_getattr(self, "ControlPointsList")))) && ((sizeof_(express_getitem(weightsdata, 1 - 1))) == (sizeof_(express_getitem(express_getattr(self, "ControlPointsList"), 1 - 1))))).truthy()) return false;
    return true;
}

bool IfcRationalBSplineSurfaceWithKnots_WeightValuesGreaterZero(EntityRef self) {
    if (!Value(IfcSurfaceWeightsPositive(self)).truthy()) return false;
    return true;
}

Value calc_IfcRationalBSplineSurfaceWithKnots_Weights(EntityRef self) {
    Value uupper = express_getattr(self, "UUpper");
    Value vupper = express_getattr(self, "VUpper");
    Value weightsdata = express_getattr(self, "WeightsData");
    return IfcMakeArrayOfArray(weightsdata, 0, uupper, 0, vupper);
}

bool IfcRectangleHollowProfileDef_ValidWallThickness(EntityRef self) {
    Value wallthickness = express_getattr(self, "WallThickness");
    if (!Value((wallthickness < ((express_getattr(self, "XDim")) / 2.)) && (wallthickness < ((express_getattr(self, "YDim")) / 2.))).truthy()) return false;
    return true;
}

bool IfcRectangleHollowProfileDef_ValidInnerRadius(EntityRef self) {
    Value wallthickness = express_getattr(self, "WallThickness");
    Value innerfilletradius = express_getattr(self, "InnerFilletRadius");
    if (!Value(!(exists(innerfilletradius)) || ((innerfilletradius <= (((express_getattr(self, "XDim")) / 2.) - wallthickness)) && (innerfilletradius <= (((express_getattr(self, "YDim")) / 2.) - wallthickness)))).truthy()) return false;
    return true;
}

bool IfcRectangleHollowProfileDef_ValidOuterRadius(EntityRef self) {
    Value outerfilletradius = express_getattr(self, "OuterFilletRadius");
    if (!Value(!(exists(outerfilletradius)) || ((outerfilletradius <= ((express_getattr(self, "XDim")) / 2.)) && (outerfilletradius <= ((express_getattr(self, "YDim")) / 2.)))).truthy()) return false;
    return true;
}

bool IfcRectangularTrimmedSurface_U1AndU2Different(EntityRef self) {
    Value u1 = express_getattr(self, "U1");
    Value u2 = express_getattr(self, "U2");
    if (!Value(u1 != u2).truthy()) return false;
    return true;
}

bool IfcRectangularTrimmedSurface_V1AndV2Different(EntityRef self) {
    Value v1 = express_getattr(self, "V1");
    Value v2 = express_getattr(self, "V2");
    if (!Value(v1 != v2).truthy()) return false;
    return true;
}

bool IfcRectangularTrimmedSurface_UsenseCompatible(EntityRef self) {
    Value basissurface = express_getattr(self, "BasisSurface");
    Value u1 = express_getattr(self, "U1");
    Value u2 = express_getattr(self, "U2");
    Value usense = express_getattr(self, "Usense");
    if (!Value(((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcelementarysurface")), typeof_(basissurface))) && (!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcplane")), typeof_(basissurface)))))) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcsurfaceofrevolution")), typeof_(basissurface))) || (usense == (u2 > u1))).truthy()) return false;
    return true;
}

bool IfcRectangularTrimmedSurface_VsenseCompatible(EntityRef self) {
    Value v1 = express_getattr(self, "V1");
    Value v2 = express_getattr(self, "V2");
    Value vsense = express_getattr(self, "Vsense");
    if (!Value(vsense == (v2 > v1)).truthy()) return false;
    return true;
}

bool IfcReinforcingBar_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcReinforcingBar_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcreinforcingbartype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcReinforcingBarType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcReinforcingBarType_BendingShapeCodeProvided(EntityRef self) {
    Value bendingshapecode = express_getattr(self, "BendingShapeCode");
    Value bendingparameters = express_getattr(self, "BendingParameters");
    if (!Value(!(exists(bendingparameters)) || exists(bendingshapecode)).truthy()) return false;
    return true;
}

bool IfcReinforcingMesh_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcReinforcingMesh_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcreinforcingmeshtype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcReinforcingMeshType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcReinforcingMeshType_BendingShapeCodeProvided(EntityRef self) {
    Value bendingshapecode = express_getattr(self, "BendingShapeCode");
    Value bendingparameters = express_getattr(self, "BendingParameters");
    if (!Value(!(exists(bendingparameters)) || exists(bendingshapecode)).truthy()) return false;
    return true;
}

bool IfcRelAggregates_NoSelfReference(EntityRef self) {
    Value relatingobject = express_getattr(self, "RelatingObject");
    Value relatedobjects = express_getattr(self, "RelatedObjects");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(relatedobjects)) { if (Value(relatingobject == temp).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelAssigns_WR1(EntityRef self) {
    Value relatedobjects = express_getattr(self, "RelatedObjects");
    Value relatedobjectstype = express_getattr(self, "RelatedObjectsType");
    if (!Value(IfcCorrectObjectAssignment(relatedobjectstype, relatedobjects)).truthy()) return false;
    return true;
}

bool IfcRelAssignsToActor_NoSelfReference(EntityRef self) {
    Value relatingactor = express_getattr(self, "RelatingActor");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "RelatedObjects"))) { if (Value(relatingactor == temp).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelAssignsToControl_NoSelfReference(EntityRef self) {
    Value relatingcontrol = express_getattr(self, "RelatingControl");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "RelatedObjects"))) { if (Value(relatingcontrol == temp).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelAssignsToGroup_NoSelfReference(EntityRef self) {
    Value relatinggroup = express_getattr(self, "RelatingGroup");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "RelatedObjects"))) { if (Value(relatinggroup == temp).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelAssignsToProcess_NoSelfReference(EntityRef self) {
    Value relatingprocess = express_getattr(self, "RelatingProcess");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "RelatedObjects"))) { if (Value(relatingprocess == temp).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelAssignsToProduct_NoSelfReference(EntityRef self) {
    Value relatingproduct = express_getattr(self, "RelatingProduct");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "RelatedObjects"))) { if (Value(relatingproduct == temp).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelAssignsToResource_NoSelfReference(EntityRef self) {
    Value relatingresource = express_getattr(self, "RelatingResource");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "RelatedObjects"))) { if (Value(relatingresource == temp).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelAssociatesMaterial_NoVoidElement(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "RelatedObjects"))) { if (Value((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcfeatureelementsubtraction")), typeof_(temp))) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcvirtualelement")), typeof_(temp)))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelAssociatesMaterial_AllowedElements(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "RelatedObjects"))) { if (Value((sizeof_(typeof_(temp) * (Value::make_list({Value(std::string("ifc4x1.ifcelement")), Value(std::string("ifc4x1.ifcelementtype")), Value(std::string("ifc4x1.ifcwindowstyle")), Value(std::string("ifc4x1.ifcdoorstyle")), Value(std::string("ifc4x1.ifcstructuralmember")), Value(std::string("ifc4x1.ifcport"))})))) == 0).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelConnectsElements_NoSelfReference(EntityRef self) {
    Value relatingelement = express_getattr(self, "RelatingElement");
    Value relatedelement = express_getattr(self, "RelatedElement");
    if (!Value(relatingelement != relatedelement).truthy()) return false;
    return true;
}

bool IfcRelConnectsPathElements_NormalizedRelatingPriorities(EntityRef self) {
    Value relatingpriorities = express_getattr(self, "RelatingPriorities");
    if (!Value((sizeof_(relatingpriorities) == 0) || ((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(relatingpriorities)) { if (Value(0 <= temp <= 100).truthy()) __r.append(temp); } return __r; })())) == sizeof_(relatingpriorities))).truthy()) return false;
    return true;
}

bool IfcRelConnectsPathElements_NormalizedRelatedPriorities(EntityRef self) {
    Value relatedpriorities = express_getattr(self, "RelatedPriorities");
    if (!Value((sizeof_(relatedpriorities) == 0) || ((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(relatedpriorities)) { if (Value(0 <= temp <= 100).truthy()) __r.append(temp); } return __r; })())) == sizeof_(relatedpriorities))).truthy()) return false;
    return true;
}

bool IfcRelConnectsPorts_NoSelfReference(EntityRef self) {
    Value relatingport = express_getattr(self, "RelatingPort");
    Value relatedport = express_getattr(self, "RelatedPort");
    if (!Value(relatingport != relatedport).truthy()) return false;
    return true;
}

bool IfcRelContainedInSpatialStructure_WR31(EntityRef self) {
    Value relatedelements = express_getattr(self, "RelatedElements");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(relatedelements)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcspatialstructureelement")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelDeclares_NoSelfReference(EntityRef self) {
    Value relatingcontext = express_getattr(self, "RelatingContext");
    Value relateddefinitions = express_getattr(self, "RelatedDefinitions");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(relateddefinitions)) { if (Value(relatingcontext == temp).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelDefinesByProperties_NoRelatedTypeObject(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& types : ifcapi::express::iter(express_getattr(self, "RelatedObjects"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifctypeobject")), typeof_(types))).truthy()) __r.append(types); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelInterferesElements_NotSelfReference(EntityRef self) {
    Value relatingelement = express_getattr(self, "RelatingElement");
    Value relatedelement = express_getattr(self, "RelatedElement");
    if (!Value(relatingelement != relatedelement).truthy()) return false;
    return true;
}

bool IfcRelNests_NoSelfReference(EntityRef self) {
    Value relatingobject = express_getattr(self, "RelatingObject");
    Value relatedobjects = express_getattr(self, "RelatedObjects");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(relatedobjects)) { if (Value(relatingobject == temp).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelReferencedInSpatialStructure_AllowedRelatedElements(EntityRef self) {
    Value relatedelements = express_getattr(self, "RelatedElements");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(relatedelements)) { if (Value((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcspatialstructureelement")), typeof_(temp))) && (!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcspace")), typeof_(temp)))))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelSequence_AvoidInconsistentSequence(EntityRef self) {
    Value relatingprocess = express_getattr(self, "RelatingProcess");
    Value relatedprocess = express_getattr(self, "RelatedProcess");
    if (!Value(relatingprocess != relatedprocess).truthy()) return false;
    return true;
}

bool IfcRelSequence_CorrectSequenceType(EntityRef self) {
    Value sequencetype = express_getattr(self, "SequenceType");
    Value userdefinedsequencetype = express_getattr(self, "UserDefinedSequenceType");
    if (!Value((sequencetype != Value(std::string("USERDEFINED"))) || ((sequencetype == Value(std::string("USERDEFINED"))) && exists(userdefinedsequencetype))).truthy()) return false;
    return true;
}

bool IfcRelSpaceBoundary_CorrectPhysOrVirt(EntityRef self) {
    Value relatedbuildingelement = express_getattr(self, "RelatedBuildingElement");
    Value physicalorvirtualboundary = express_getattr(self, "PhysicalOrVirtualBoundary");
    if (!Value(((physicalorvirtualboundary == Value(std::string("Physical"))) && (!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcvirtualelement")), typeof_(relatedbuildingelement)))))) || ((physicalorvirtualboundary == Value(std::string("Virtual"))) && ((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcvirtualelement")), typeof_(relatedbuildingelement))) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcopeningelement")), typeof_(relatedbuildingelement))))) || (physicalorvirtualboundary == Value(std::string("NotDefined")))).truthy()) return false;
    return true;
}

bool IfcReparametrisedCompositeCurveSegment_PositiveLengthParameter(EntityRef self) {
    Value paramlength = express_getattr(self, "ParamLength");
    if (!Value(paramlength > 0.0).truthy()) return false;
    return true;
}

bool IfcRepresentationMap_ApplicableMappedRepr(EntityRef self) {
    Value mappedrepresentation = express_getattr(self, "MappedRepresentation");
    if (!Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcshapemodel")), typeof_(mappedrepresentation))).truthy()) return false;
    return true;
}

bool IfcRevolvedAreaSolid_AxisStartInXY(EntityRef self) {
    Value axis = express_getattr(self, "Axis");
    if (!Value((express_getitem(express_getattr(express_getattr(axis, "Location"), "Coordinates"), 3 - 1)) == 0.0).truthy()) return false;
    return true;
}

bool IfcRevolvedAreaSolid_AxisDirectionInXY(EntityRef self) {
    Value axis = express_getattr(self, "Axis");
    if (!Value((express_getitem(express_getattr(express_getattr(axis, "Z"), "DirectionRatios"), 3 - 1)) == 0.0).truthy()) return false;
    return true;
}

Value calc_IfcRevolvedAreaSolid_AxisLine(EntityRef self) {
    Value axis = express_getattr(self, "Axis");
    return IfcLine(express_getattr(axis, "Location"),  IfcVector(express_getattr(axis, "Z"),  1.0));
}

bool IfcRevolvedAreaSolidTapered_CorrectProfileAssignment(EntityRef self) {
    if (!Value(IfcTaperedSweptAreaProfiles(express_getattr(self, "SweptArea"), express_getattr(self, "EndSweptArea"))).truthy()) return false;
    return true;
}

bool IfcRoof_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcRoof_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcrooftype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcRoofType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcRoundedRectangleProfileDef_ValidRadius(EntityRef self) {
    Value roundingradius = express_getattr(self, "RoundingRadius");
    if (!Value((roundingradius <= ((express_getattr(self, "XDim")) / 2.)) && (roundingradius <= ((express_getattr(self, "YDim")) / 2.))).truthy()) return false;
    return true;
}

Value calc_IfcSIUnit_Dimensions(EntityRef self) {
    return IfcDimensionsForSiUnit(express_getattr(self, "Name"));
}

bool IfcSanitaryTerminal_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcSanitaryTerminal_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcsanitaryterminaltype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcSanitaryTerminalType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcSeamCurve_TwoPCurves(EntityRef self) {
    if (!Value((sizeof_(express_getattr(self, "AssociatedGeometry"))) == 2).truthy()) return false;
    return true;
}

bool IfcSeamCurve_SameSurface(EntityRef self) {
    if (!Value((IfcAssociatedSurface(express_getitem(express_getattr(self, "AssociatedGeometry"), 1 - 1))) == (IfcAssociatedSurface(express_getitem(express_getattr(self, "AssociatedGeometry"), 2 - 1)))).truthy()) return false;
    return true;
}

bool IfcSectionedSolid_DirectrixIs3D(EntityRef self) {
    Value directrix = express_getattr(self, "Directrix");
    if (!Value((express_getattr(directrix, "Dim")) == 3).truthy()) return false;
    return true;
}

bool IfcSectionedSolid_ConsistentProfileTypes(EntityRef self) {
    Value crosssections = express_getattr(self, "CrossSections");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(crosssections)) { if (Value((express_getattr(express_getitem(crosssections, 1 - 1), "ProfileType")) != (express_getattr(temp, "ProfileType"))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcSectionedSolid_SectionsSameType(EntityRef self) {
    Value crosssections = express_getattr(self, "CrossSections");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(crosssections)) { if (Value((typeof_(express_getitem(crosssections, 1 - 1))) != typeof_(temp)).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcSectionedSolidHorizontal_CorrespondingSectionPositions(EntityRef self) {
    Value crosssections = express_getattr(self, "CrossSections");
    Value crosssectionpositions = express_getattr(self, "CrossSectionPositions");
    if (!Value(sizeof_(crosssections) == sizeof_(crosssectionpositions)).truthy()) return false;
    return true;
}

bool IfcSectionedSolidHorizontal_NoLongitudinalOffsets(EntityRef self) {
    Value crosssectionpositions = express_getattr(self, "CrossSectionPositions");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(crosssectionpositions)) { if (Value(exists(express_getattr(temp, "OffsetLongitudinal"))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcSectionedSpine_CorrespondingSectionPositions(EntityRef self) {
    Value crosssections = express_getattr(self, "CrossSections");
    Value crosssectionpositions = express_getattr(self, "CrossSectionPositions");
    if (!Value(sizeof_(crosssections) == sizeof_(crosssectionpositions)).truthy()) return false;
    return true;
}

bool IfcSectionedSpine_ConsistentProfileTypes(EntityRef self) {
    Value crosssections = express_getattr(self, "CrossSections");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(crosssections)) { if (Value((express_getattr(express_getitem(crosssections, 1 - 1), "ProfileType")) != (express_getattr(temp, "ProfileType"))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcSectionedSpine_SpineCurveDim(EntityRef self) {
    Value spinecurve = express_getattr(self, "SpineCurve");
    if (!Value((express_getattr(spinecurve, "Dim")) == 3).truthy()) return false;
    return true;
}

Value calc_IfcSectionedSpine_Dim(EntityRef self) {
    return 3;
}

bool IfcSensor_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcSensor_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcsensortype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcSensorType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcShadingDevice_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcShadingDevice_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcshadingdevicetype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcShadingDeviceType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcShapeModel_WR11(EntityRef self) {
    Value ofshapeaspect = express_getattr(self, "OfShapeAspect");
    if (!Value(((sizeof_(express_getattr(self, "OfProductRepresentation"))) == 1) ^ ((sizeof_(express_getattr(self, "RepresentationMap"))) == 1) ^ (sizeof_(ofshapeaspect) == 1)).truthy()) return false;
    return true;
}

bool IfcShapeRepresentation_CorrectContext(EntityRef self) {
    if (!Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcgeometricrepresentationcontext")), typeof_(express_getattr(self, "ContextOfItems")))).truthy()) return false;
    return true;
}

bool IfcShapeRepresentation_NoTopologicalItem(EntityRef self) {
    Value items = express_getattr(self, "Items");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value((ifcapi::express::express_in(Value(std::string("ifc4x1.ifctopologicalrepresentationitem")), typeof_(temp))) && (!(((sizeof_((Value::make_list({Value(std::string("ifc4x1.ifcvertexpoint")), Value(std::string("ifc4x1.ifcedgecurve")), Value(std::string("ifc4x1.ifcfacesurface"))})) * typeof_(temp))) == 1)))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcShapeRepresentation_HasRepresentationType(EntityRef self) {
    if (!Value(exists(express_getattr(self, "RepresentationType"))).truthy()) return false;
    return true;
}

bool IfcShapeRepresentation_HasRepresentationIdentifier(EntityRef self) {
    if (!Value(exists(express_getattr(self, "RepresentationIdentifier"))).truthy()) return false;
    return true;
}

bool IfcShapeRepresentation_CorrectItemsForType(EntityRef self) {
    if (!Value(IfcShapeRepresentationTypes(express_getattr(self, "RepresentationType"), express_getattr(self, "Items"))).truthy()) return false;
    return true;
}

Value calc_IfcShellBasedSurfaceModel_Dim(EntityRef self) {
    return 3;
}

bool IfcSlab_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcSlab_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcslabtype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcSlabElementedCase_HasDecomposition(EntityRef self) {
    if (!Value((hiindex(express_getattr(self, "IsDecomposedBy"))) > 0).truthy()) return false;
    return true;
}

bool IfcSlabStandardCase_HasMaterialLayerSetusage(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(usedin(self, Value(std::string("ifc4x1.ifcrelassociates.relatedobjects"))))) { if (Value((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcrelassociatesmaterial")), typeof_(temp))) && (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcmateriallayersetusage")), typeof_(express_getattr(temp, "RelatingMaterial"))))).truthy()) __r.append(temp); } return __r; })())) == 1).truthy()) return false;
    return true;
}

bool IfcSlabType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcSolarDevice_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcSolarDevice_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcsolardevicetype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcSolarDeviceType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

Value calc_IfcSolidModel_Dim(EntityRef self) {
    return 3;
}

bool IfcSpace_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcSpace_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcspacetype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcSpaceHeater_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcSpaceHeater_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcspaceheatertype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcSpaceHeaterType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcSpaceType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcSpatialStructureElement_WR41(EntityRef self) {
    if (!Value(((hiindex(express_getattr(self, "Decomposes"))) == 1) && (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcrelaggregates")), typeof_(express_getitem(express_getattr(self, "Decomposes"), 1 - 1)))) && ((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcproject")), typeof_(express_getattr(express_getitem(express_getattr(self, "Decomposes"), 1 - 1), "RelatingObject")))) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcspatialstructureelement")), typeof_(express_getattr(express_getitem(express_getattr(self, "Decomposes"), 1 - 1), "RelatingObject")))))).truthy()) return false;
    return true;
}

bool IfcSpatialZone_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcSpatialZone_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcspatialzonetype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcSpatialZoneType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcStackTerminal_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcStackTerminal_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcstackterminaltype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcStackTerminalType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcStair_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcStair_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcstairtype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcStairFlight_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcStairFlight_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcstairflighttype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcStairFlightType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcStairType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcStructuralAnalysisModel_HasObjectType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || (exists(express_getattr(self, "ObjectType")))).truthy()) return false;
    return true;
}

bool IfcStructuralCurveAction_ProjectedIsGlobal(EntityRef self) {
    Value projectedortrue = express_getattr(self, "ProjectedOrTrue");
    if (!Value(!(exists(projectedortrue)) || ((projectedortrue != projected_length) || ((express_getattr(self, "GlobalOrLocal")) == global_coords))).truthy()) return false;
    return true;
}

bool IfcStructuralCurveAction_HasObjectType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || (exists(express_getattr(self, "ObjectType")))).truthy()) return false;
    return true;
}

bool IfcStructuralCurveAction_SuitablePredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(predefinedtype != Value(std::string("EQUIDISTANT"))).truthy()) return false;
    return true;
}

bool IfcStructuralCurveMember_HasObjectType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || (exists(express_getattr(self, "ObjectType")))).truthy()) return false;
    return true;
}

bool IfcStructuralCurveReaction_HasObjectType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || (exists(express_getattr(self, "ObjectType")))).truthy()) return false;
    return true;
}

bool IfcStructuralCurveReaction_SuitablePredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("SINUS"))) && (predefinedtype != Value(std::string("PARABOLA")))).truthy()) return false;
    return true;
}

bool IfcStructuralLinearAction_SuitableLoadType(EntityRef self) {
    if (!Value((sizeof_((Value::make_list({Value(std::string("ifc4x1.ifcstructuralloadlinearforce")), Value(std::string("ifc4x1.ifcstructuralloadtemperature"))})) * (typeof_(express_getattr(self, "AppliedLoad"))))) == 1).truthy()) return false;
    return true;
}

bool IfcStructuralLinearAction_ConstPredefinedType(EntityRef self) {
    if (!Value((express_getattr(self, "PredefinedType")) == Value(std::string("CONST"))).truthy()) return false;
    return true;
}

bool IfcStructuralLoadCase_IsLoadCasePredefinedType(EntityRef self) {
    if (!Value((express_getattr(self, "PredefinedType")) == Value(std::string("LOAD_CASE"))).truthy()) return false;
    return true;
}

bool IfcStructuralLoadConfiguration_ValidListSize(EntityRef self) {
    Value values = express_getattr(self, "Values");
    Value locations = express_getattr(self, "Locations");
    if (!Value(!(exists(locations)) || (sizeof_(locations) == sizeof_(values))).truthy()) return false;
    return true;
}

bool IfcStructuralLoadGroup_HasObjectType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    Value actiontype = express_getattr(self, "ActionType");
    Value actionsource = express_getattr(self, "ActionSource");
    if (!Value(((predefinedtype != Value(std::string("USERDEFINED"))) && (actiontype != Value(std::string("USERDEFINED"))) && (actionsource != Value(std::string("USERDEFINED")))) || (exists(express_getattr(self, "ObjectType")))).truthy()) return false;
    return true;
}

bool IfcStructuralPlanarAction_SuitableLoadType(EntityRef self) {
    if (!Value((sizeof_((Value::make_list({Value(std::string("ifc4x1.ifcstructuralloadplanarforce")), Value(std::string("ifc4x1.ifcstructuralloadtemperature"))})) * (typeof_(express_getattr(self, "AppliedLoad"))))) == 1).truthy()) return false;
    return true;
}

bool IfcStructuralPlanarAction_ConstPredefinedType(EntityRef self) {
    if (!Value((express_getattr(self, "PredefinedType")) == Value(std::string("CONST"))).truthy()) return false;
    return true;
}

bool IfcStructuralPointAction_SuitableLoadType(EntityRef self) {
    if (!Value((sizeof_((Value::make_list({Value(std::string("ifc4x1.ifcstructuralloadsingleforce")), Value(std::string("ifc4x1.ifcstructuralloadsingledisplacement"))})) * (typeof_(express_getattr(self, "AppliedLoad"))))) == 1).truthy()) return false;
    return true;
}

bool IfcStructuralPointReaction_SuitableLoadType(EntityRef self) {
    if (!Value((sizeof_((Value::make_list({Value(std::string("ifc4x1.ifcstructuralloadsingleforce")), Value(std::string("ifc4x1.ifcstructuralloadsingledisplacement"))})) * (typeof_(express_getattr(self, "AppliedLoad"))))) == 1).truthy()) return false;
    return true;
}

bool IfcStructuralResultGroup_HasObjectType(EntityRef self) {
    Value theorytype = express_getattr(self, "TheoryType");
    if (!Value((theorytype != Value(std::string("USERDEFINED"))) || (exists(express_getattr(self, "ObjectType")))).truthy()) return false;
    return true;
}

bool IfcStructuralSurfaceAction_ProjectedIsGlobal(EntityRef self) {
    Value projectedortrue = express_getattr(self, "ProjectedOrTrue");
    if (!Value(!(exists(projectedortrue)) || ((projectedortrue != projected_length) || ((express_getattr(self, "GlobalOrLocal")) == global_coords))).truthy()) return false;
    return true;
}

bool IfcStructuralSurfaceAction_HasObjectType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || (exists(express_getattr(self, "ObjectType")))).truthy()) return false;
    return true;
}

bool IfcStructuralSurfaceMember_HasObjectType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || (exists(express_getattr(self, "ObjectType")))).truthy()) return false;
    return true;
}

bool IfcStructuralSurfaceReaction_HasPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || (exists(express_getattr(self, "ObjectType")))).truthy()) return false;
    return true;
}

bool IfcStyledItem_ApplicableItem(EntityRef self) {
    Value item = express_getattr(self, "Item");
    if (!Value(!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcstyleditem")), typeof_(item))))).truthy()) return false;
    return true;
}

bool IfcStyledRepresentation_OnlyStyledItems(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "Items"))) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcstyleditem")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcSubContractResource_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcSubContractResourceType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ResourceType"))))).truthy()) return false;
    return true;
}

Value calc_IfcSurface_Dim(EntityRef self) {
    return 3;
}

bool IfcSurfaceCurve_CurveIs3D(EntityRef self) {
    Value curve3d = express_getattr(self, "Curve3D");
    if (!Value((express_getattr(curve3d, "Dim")) == 3).truthy()) return false;
    return true;
}

bool IfcSurfaceCurve_CurveIsNotPcurve(EntityRef self) {
    Value curve3d = express_getattr(self, "Curve3D");
    if (!Value(!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcpcurve")), typeof_(curve3d))))).truthy()) return false;
    return true;
}

Value calc_IfcSurfaceCurve_BasisSurface(EntityRef self) {
    return IfcGetBasisSurface(self);
}

bool IfcSurfaceCurveSweptAreaSolid_DirectrixBounded(EntityRef self) {
    Value directrix = express_getattr(self, "Directrix");
    Value startparam = express_getattr(self, "StartParam");
    Value endparam = express_getattr(self, "EndParam");
    if (!Value((exists(startparam) && exists(endparam)) || ((sizeof_((Value::make_list({Value(std::string("ifc4x1.ifcconic")), Value(std::string("ifc4x1.ifcboundedcurve"))})) * typeof_(directrix))) == 1)).truthy()) return false;
    return true;
}

bool IfcSurfaceFeature_HasObjectType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || (exists(express_getattr(self, "ObjectType")))).truthy()) return false;
    return true;
}

bool IfcSurfaceOfLinearExtrusion_DepthGreaterZero(EntityRef self) {
    Value depth = express_getattr(self, "Depth");
    if (!Value(depth > 0.).truthy()) return false;
    return true;
}

Value calc_IfcSurfaceOfLinearExtrusion_ExtrusionAxis(EntityRef self) {
    Value extrudeddirection = express_getattr(self, "ExtrudedDirection");
    Value depth = express_getattr(self, "Depth");
    return IfcVector(extrudeddirection,  depth);
}

Value calc_IfcSurfaceOfRevolution_AxisLine(EntityRef self) {
    Value axisposition = express_getattr(self, "AxisPosition");
    return IfcLine(express_getattr(axisposition, "Location"),  IfcVector(express_getattr(axisposition, "Z"),  1.0));
}

bool IfcSurfaceReinforcementArea_SurfaceAndOrShearAreaSpecified(EntityRef self) {
    Value surfacereinforcement1 = express_getattr(self, "SurfaceReinforcement1");
    Value surfacereinforcement2 = express_getattr(self, "SurfaceReinforcement2");
    Value shearreinforcement = express_getattr(self, "ShearReinforcement");
    if (!Value(exists(surfacereinforcement1) || exists(surfacereinforcement2) || exists(shearreinforcement)).truthy()) return false;
    return true;
}

bool IfcSurfaceReinforcementArea_NonnegativeArea1(EntityRef self) {
    Value surfacereinforcement1 = express_getattr(self, "SurfaceReinforcement1");
    if (!Value(!(exists(surfacereinforcement1)) || (((express_getitem(surfacereinforcement1, 1 - 1)) >= 0.) && ((express_getitem(surfacereinforcement1, 2 - 1)) >= 0.) && ((sizeof_(surfacereinforcement1) == 1) || ((express_getitem(surfacereinforcement1, 1 - 1)) >= 0.)))).truthy()) return false;
    return true;
}

bool IfcSurfaceReinforcementArea_NonnegativeArea2(EntityRef self) {
    Value surfacereinforcement2 = express_getattr(self, "SurfaceReinforcement2");
    if (!Value(!(exists(surfacereinforcement2)) || (((express_getitem(surfacereinforcement2, 1 - 1)) >= 0.) && ((express_getitem(surfacereinforcement2, 2 - 1)) >= 0.) && ((sizeof_(surfacereinforcement2) == 1) || ((express_getitem(surfacereinforcement2, 1 - 1)) >= 0.)))).truthy()) return false;
    return true;
}

bool IfcSurfaceReinforcementArea_NonnegativeArea3(EntityRef self) {
    Value shearreinforcement = express_getattr(self, "ShearReinforcement");
    if (!Value(!(exists(shearreinforcement)) || (shearreinforcement >= 0.)).truthy()) return false;
    return true;
}

bool IfcSurfaceStyle_MaxOneShading(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& style : ifcapi::express::iter(express_getattr(self, "Styles"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcsurfacestyleshading")), typeof_(style))).truthy()) __r.append(style); } return __r; })())) <= 1).truthy()) return false;
    return true;
}

bool IfcSurfaceStyle_MaxOneLighting(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& style : ifcapi::express::iter(express_getattr(self, "Styles"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcsurfacestylelighting")), typeof_(style))).truthy()) __r.append(style); } return __r; })())) <= 1).truthy()) return false;
    return true;
}

bool IfcSurfaceStyle_MaxOneRefraction(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& style : ifcapi::express::iter(express_getattr(self, "Styles"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcsurfacestylerefraction")), typeof_(style))).truthy()) __r.append(style); } return __r; })())) <= 1).truthy()) return false;
    return true;
}

bool IfcSurfaceStyle_MaxOneTextures(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& style : ifcapi::express::iter(express_getattr(self, "Styles"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcsurfacestylewithtextures")), typeof_(style))).truthy()) __r.append(style); } return __r; })())) <= 1).truthy()) return false;
    return true;
}

bool IfcSurfaceStyle_MaxOneExtDefined(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& style : ifcapi::express::iter(express_getattr(self, "Styles"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc4x1.ifcexternallydefinedsurfacestyle")), typeof_(style))).truthy()) __r.append(style); } return __r; })())) <= 1).truthy()) return false;
    return true;
}

bool IfcSweptAreaSolid_SweptAreaType(EntityRef self) {
    Value sweptarea = express_getattr(self, "SweptArea");
    if (!Value((express_getattr(sweptarea, "ProfileType")) == Value(std::string("Area"))).truthy()) return false;
    return true;
}

bool IfcSweptDiskSolid_DirectrixDim(EntityRef self) {
    Value directrix = express_getattr(self, "Directrix");
    if (!Value((express_getattr(directrix, "Dim")) == 3).truthy()) return false;
    return true;
}

bool IfcSweptDiskSolid_InnerRadiusSize(EntityRef self) {
    Value radius = express_getattr(self, "Radius");
    Value innerradius = express_getattr(self, "InnerRadius");
    if (!Value(!(exists(innerradius)) || (radius > innerradius)).truthy()) return false;
    return true;
}

bool IfcSweptDiskSolid_DirectrixBounded(EntityRef self) {
    Value directrix = express_getattr(self, "Directrix");
    Value startparam = express_getattr(self, "StartParam");
    Value endparam = express_getattr(self, "EndParam");
    if (!Value((exists(startparam) && exists(endparam)) || ((sizeof_((Value::make_list({Value(std::string("ifc4x1.ifcconic")), Value(std::string("ifc4x1.ifcboundedcurve"))})) * typeof_(directrix))) == 1)).truthy()) return false;
    return true;
}

bool IfcSweptDiskSolidPolygonal_CorrectRadii(EntityRef self) {
    Value filletradius = express_getattr(self, "FilletRadius");
    if (!Value(!(exists(filletradius)) || (filletradius >= (express_getattr(self, "Radius")))).truthy()) return false;
    return true;
}

bool IfcSweptDiskSolidPolygonal_DirectrixIsPolyline(EntityRef self) {
    if (!Value((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcpolyline")), typeof_(express_getattr(self, "Directrix")))) || ((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcindexedpolycurve")), typeof_(express_getattr(self, "Directrix")))) && (!((exists(express_getattr(express_getattr(self, "Directrix"), "Segments"))))))).truthy()) return false;
    return true;
}

bool IfcSweptSurface_SweptCurveType(EntityRef self) {
    Value sweptcurve = express_getattr(self, "SweptCurve");
    if (!Value((express_getattr(sweptcurve, "ProfileType")) == Value(std::string("Curve"))).truthy()) return false;
    return true;
}

bool IfcSwitchingDevice_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcSwitchingDevice_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcswitchingdevicetype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcSwitchingDeviceType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcSystemFurnitureElement_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcSystemFurnitureElement_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcsystemfurnitureelementtype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcSystemFurnitureElementType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcTShapeProfileDef_ValidFlangeThickness(EntityRef self) {
    Value depth = express_getattr(self, "Depth");
    Value flangethickness = express_getattr(self, "FlangeThickness");
    if (!Value(flangethickness < depth).truthy()) return false;
    return true;
}

bool IfcTShapeProfileDef_ValidWebThickness(EntityRef self) {
    Value flangewidth = express_getattr(self, "FlangeWidth");
    Value webthickness = express_getattr(self, "WebThickness");
    if (!Value(webthickness < flangewidth).truthy()) return false;
    return true;
}

bool IfcTable_WR1(EntityRef self) {
    Value rows = express_getattr(self, "Rows");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(rows)) { if (Value((hiindex(express_getattr(temp, "RowCells"))) != (hiindex(express_getattr(express_getitem(rows, 1 - 1), "RowCells")))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcTable_WR2(EntityRef self) {
    Value numberofheadings = express_getattr(self, "NumberOfHeadings");
    if (!Value(0 <= numberofheadings <= 1).truthy()) return false;
    return true;
}

Value calc_IfcTable_NumberOfCellsInRow(EntityRef self) {
    Value rows = express_getattr(self, "Rows");
    return hiindex(express_getattr(express_getitem(rows, 1 - 1), "RowCells"));
}

Value calc_IfcTable_NumberOfHeadings(EntityRef self) {
    Value rows = express_getattr(self, "Rows");
    return sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(rows)) { if (Value(express_getattr(temp, "IsHeading")).truthy()) __r.append(temp); } return __r; })());
}

Value calc_IfcTable_NumberOfDataRows(EntityRef self) {
    Value rows = express_getattr(self, "Rows");
    return sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(rows)) { if (Value(!((express_getattr(temp, "IsHeading")))).truthy()) __r.append(temp); } return __r; })());
}

bool IfcTank_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcTank_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifctanktype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcTankType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcTask_HasName(EntityRef self) {
    if (!Value(exists(express_getattr(self, "Name"))).truthy()) return false;
    return true;
}

bool IfcTask_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcTaskType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ProcessType"))))).truthy()) return false;
    return true;
}

bool IfcTelecomAddress_MinimumDataProvided(EntityRef self) {
    Value telephonenumbers = express_getattr(self, "TelephoneNumbers");
    Value facsimilenumbers = express_getattr(self, "FacsimileNumbers");
    Value pagernumber = express_getattr(self, "PagerNumber");
    Value electronicmailaddresses = express_getattr(self, "ElectronicMailAddresses");
    Value wwwhomepageurl = express_getattr(self, "WWWHomePageURL");
    Value messagingids = express_getattr(self, "MessagingIDs");
    if (!Value(exists(telephonenumbers) || exists(facsimilenumbers) || exists(pagernumber) || exists(electronicmailaddresses) || exists(wwwhomepageurl) || exists(messagingids)).truthy()) return false;
    return true;
}

bool IfcTendon_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcTendon_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifctendontype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcTendonAnchor_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcTendonAnchor_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifctendonanchortype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcTendonAnchorType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcTendonType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

Value calc_IfcTessellatedFaceSet_Dim(EntityRef self) {
    return 3;
}

bool IfcTextLiteralWithExtent_WR31(EntityRef self) {
    Value extent = express_getattr(self, "Extent");
    if (!Value(!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcplanarbox")), typeof_(extent))))).truthy()) return false;
    return true;
}

bool IfcTextStyleFontModel_MeasureOfFontSize(EntityRef self) {
    if (!Value((ifcapi::express::express_in(Value(std::string("ifc4x1.ifclengthmeasure")), typeof_(express_getattr(self, "FontSize")))) && ((express_getattr(self, "FontSize")) > 0.)).truthy()) return false;
    return true;
}

bool IfcTopologyRepresentation_WR21(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "Items"))) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifctopologicalrepresentationitem")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcTopologyRepresentation_WR22(EntityRef self) {
    if (!Value(exists(express_getattr(self, "RepresentationType"))).truthy()) return false;
    return true;
}

bool IfcTopologyRepresentation_WR23(EntityRef self) {
    if (!Value(IfcTopologyRepresentationTypes(express_getattr(self, "RepresentationType"), express_getattr(self, "Items"))).truthy()) return false;
    return true;
}

bool IfcToroidalSurface_MajorLargerMinor(EntityRef self) {
    Value majorradius = express_getattr(self, "MajorRadius");
    Value minorradius = express_getattr(self, "MinorRadius");
    if (!Value(minorradius < majorradius).truthy()) return false;
    return true;
}

bool IfcTransformer_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcTransformer_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifctranformertype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcTransformerType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcTransportElement_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcTransportElement_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifctransportelementtype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcTransportElementType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

Value calc_IfcTriangulatedFaceSet_NumberOfTriangles(EntityRef self) {
    Value coordindex = express_getattr(self, "CoordIndex");
    return sizeof_(coordindex);
}

bool IfcTriangulatedIrregularNetwork_NotClosed(EntityRef self) {
    if (!Value((express_getattr(self, "Closed")) == false).truthy()) return false;
    return true;
}

bool IfcTrimmedCurve_Trim1ValuesConsistent(EntityRef self) {
    Value trim1 = express_getattr(self, "Trim1");
    if (!Value((hiindex(trim1) == 1) || ((typeof_(express_getitem(trim1, 1 - 1))) != (typeof_(express_getitem(trim1, 2 - 1))))).truthy()) return false;
    return true;
}

bool IfcTrimmedCurve_Trim2ValuesConsistent(EntityRef self) {
    Value trim2 = express_getattr(self, "Trim2");
    if (!Value((hiindex(trim2) == 1) || ((typeof_(express_getitem(trim2, 1 - 1))) != (typeof_(express_getitem(trim2, 2 - 1))))).truthy()) return false;
    return true;
}

bool IfcTrimmedCurve_NoTrimOfBoundedCurves(EntityRef self) {
    Value basiscurve = express_getattr(self, "BasisCurve");
    if (!Value(!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcboundedcurve")), typeof_(basiscurve))))).truthy()) return false;
    return true;
}

bool IfcTubeBundle_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcTubeBundle_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifctubebundletype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcTubeBundleType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcTypeObject_NameRequired(EntityRef self) {
    if (!Value(exists(express_getattr(self, "Name"))).truthy()) return false;
    return true;
}

bool IfcTypeObject_UniquePropertySetNames(EntityRef self) {
    Value haspropertysets = express_getattr(self, "HasPropertySets");
    if (!Value(!(exists(haspropertysets)) || IfcUniquePropertySetNames(haspropertysets)).truthy()) return false;
    return true;
}

bool IfcTypeProduct_ApplicableOccurrence(EntityRef self) {
    if (!Value((!((exists(express_getitem(express_getattr(self, "Types"), 1 - 1))))) || ((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(express_getitem(express_getattr(self, "Types"), 1 - 1), "RelatedObjects"))) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcproduct")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })())) == 0)).truthy()) return false;
    return true;
}

bool IfcUShapeProfileDef_ValidFlangeThickness(EntityRef self) {
    Value depth = express_getattr(self, "Depth");
    Value flangethickness = express_getattr(self, "FlangeThickness");
    if (!Value(flangethickness < (depth / 2.)).truthy()) return false;
    return true;
}

bool IfcUShapeProfileDef_ValidWebThickness(EntityRef self) {
    Value flangewidth = express_getattr(self, "FlangeWidth");
    Value webthickness = express_getattr(self, "WebThickness");
    if (!Value(webthickness < flangewidth).truthy()) return false;
    return true;
}

bool IfcUnitAssignment_WR01(EntityRef self) {
    Value units = express_getattr(self, "Units");
    if (!Value(IfcCorrectUnitAssignment(units)).truthy()) return false;
    return true;
}

bool IfcUnitaryControlElement_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcUnitaryControlElement_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcunitarycontrolelementtype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcUnitaryControlElementType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcUnitaryEquipment_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcUnitaryEquipment_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcunitaryequipmenttype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcUnitaryEquipmentType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcValve_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcValve_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcvalvetype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcValveType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcVector_MagGreaterOrEqualZero(EntityRef self) {
    Value magnitude = express_getattr(self, "Magnitude");
    if (!Value(magnitude >= 0.0).truthy()) return false;
    return true;
}

Value calc_IfcVector_Dim(EntityRef self) {
    Value orientation = express_getattr(self, "Orientation");
    return express_getattr(orientation, "Dim");
}

bool IfcVibrationIsolator_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcVibrationIsolator_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcvibrationisolatortype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcVibrationIsolatorType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcVoidingFeature_HasObjectType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || (exists(express_getattr(self, "ObjectType")))).truthy()) return false;
    return true;
}

bool IfcWall_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcWall_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcwalltype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcWallElementedCase_HasDecomposition(EntityRef self) {
    if (!Value((hiindex(express_getattr(self, "IsDecomposedBy"))) > 0).truthy()) return false;
    return true;
}

bool IfcWallStandardCase_HasMaterialLayerSetUsage(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(usedin(self, Value(std::string("ifc4x1.ifcrelassociates.relatedobjects"))))) { if (Value((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcrelassociatesmaterial")), typeof_(temp))) && (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcmateriallayersetusage")), typeof_(express_getattr(temp, "RelatingMaterial"))))).truthy()) __r.append(temp); } return __r; })())) == 1).truthy()) return false;
    return true;
}

bool IfcWallType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcWasteTerminal_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcWasteTerminal_CorrectTypeAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcwasteterminaltype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcWasteTerminalType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcWindow_CorrectStyleAssigned(EntityRef self) {
    Value istypedby = express_getattr(self, "IsTypedBy");
    if (!Value((sizeof_(istypedby) == 0) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcwindowtype")), typeof_(express_getattr(express_getitem(express_getattr(self, "IsTypedBy"), 1 - 1), "RelatingType"))))).truthy()) return false;
    return true;
}

bool IfcWindowLiningProperties_WR31(EntityRef self) {
    Value liningdepth = express_getattr(self, "LiningDepth");
    Value liningthickness = express_getattr(self, "LiningThickness");
    if (!Value(!((exists(liningdepth) && !(exists(liningthickness))))).truthy()) return false;
    return true;
}

bool IfcWindowLiningProperties_WR32(EntityRef self) {
    Value firsttransomoffset = express_getattr(self, "FirstTransomOffset");
    Value secondtransomoffset = express_getattr(self, "SecondTransomOffset");
    if (!Value(!((!(exists(firsttransomoffset)) && exists(secondtransomoffset)))).truthy()) return false;
    return true;
}

bool IfcWindowLiningProperties_WR33(EntityRef self) {
    Value firstmullionoffset = express_getattr(self, "FirstMullionOffset");
    Value secondmullionoffset = express_getattr(self, "SecondMullionOffset");
    if (!Value(!((!(exists(firstmullionoffset)) && exists(secondmullionoffset)))).truthy()) return false;
    return true;
}

bool IfcWindowLiningProperties_WR34(EntityRef self) {
    if (!Value((exists(express_getitem(express_getattr(self, "DefinesType"), 1 - 1))) && ((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcwindowtype")), typeof_(express_getitem(express_getattr(self, "DefinesType"), 1 - 1)))) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcwindowstyle")), typeof_(express_getitem(express_getattr(self, "DefinesType"), 1 - 1)))))).truthy()) return false;
    return true;
}

bool IfcWindowPanelProperties_ApplicableToType(EntityRef self) {
    if (!Value((exists(express_getitem(express_getattr(self, "DefinesType"), 1 - 1))) && ((ifcapi::express::express_in(Value(std::string("ifc4x1.ifcwindowtype")), typeof_(express_getitem(express_getattr(self, "DefinesType"), 1 - 1)))) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcwindowstyle")), typeof_(express_getitem(express_getattr(self, "DefinesType"), 1 - 1)))))).truthy()) return false;
    return true;
}

bool IfcWindowType_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcWorkCalendar_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcWorkPlan_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcWorkSchedule_CorrectPredefinedType(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcZShapeProfileDef_ValidFlangeThickness(EntityRef self) {
    Value depth = express_getattr(self, "Depth");
    Value flangethickness = express_getattr(self, "FlangeThickness");
    if (!Value(flangethickness < (depth / 2.)).truthy()) return false;
    return true;
}

bool IfcZone_WR1(EntityRef self) {
    if (!Value(((sizeof_(express_getattr(self, "IsGroupedBy"))) == 0) || ((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(express_getitem(express_getattr(self, "IsGroupedBy"), 1 - 1), "RelatedObjects"))) { if (Value(!(((ifcapi::express::express_in(Value(std::string("ifc4x1.ifczone")), typeof_(temp))) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcspace")), typeof_(temp))) || (ifcapi::express::express_in(Value(std::string("ifc4x1.ifcspatialzone")), typeof_(temp)))))).truthy()) __r.append(temp); } return __r; })())) == 0)).truthy()) return false;
    return true;
}

bool IfcRepresentationContextSameWCS(IfcFile* file) {
    Value IfcGeometricRepresentationContext = ifcapi::express::file_by_type(file, "IfcGeometricRepresentationContext");
    Value isdifferent = false;
    if (Value(sizeof_(IfcGeometricRepresentationContext) > 1).truthy()) {
        for (Value i = 2; (Value(i) <= Value(hiindex(IfcGeometricRepresentationContext))).truthy(); i = i + Value((int64_t)1)) {
            if (Value((express_getattr(express_getitem(IfcGeometricRepresentationContext, 1 - 1), "WorldCoordinateSystem")) != (express_getattr(express_getitem(IfcGeometricRepresentationContext, i - 1), "WorldCoordinateSystem"))).truthy()) {
                isdifferent = (!((IfcSameValidPrecision(express_getattr(express_getitem(IfcGeometricRepresentationContext, 1 - 1), "Precision"), express_getattr(express_getitem(IfcGeometricRepresentationContext, i - 1), "Precision"))))) || (!((IfcSameAxis2Placement(express_getattr(express_getitem(IfcGeometricRepresentationContext, 1 - 1), "WorldCoordinateSystem"), express_getattr(express_getitem(IfcGeometricRepresentationContext, i - 1), "WorldCoordinateSystem"), express_getattr(express_getitem(IfcGeometricRepresentationContext, 1 - 1), "Precision")))));
                if (Value(isdifferent == true).truthy()) {
                    break;
                }
            }
        }
    }
    if (!Value(isdifferent == false).truthy()) return false;
    return true;
}

bool IfcSingleProjectInstance(IfcFile* file) {
    Value IfcProject = ifcapi::express::file_by_type(file, "IfcProject");
    if (!Value(sizeof_(IfcProject) <= 1).truthy()) return false;
    return true;
}


// --- derived-attribute registry ---
namespace { struct __derived_register_t { __derived_register_t() {
    register_derived("IFC4X1", "IfcAxis1Placement", "Z", (DeriveFn)&calc_IfcAxis1Placement_Z);
    register_derived("IFC4X1", "IfcAxis2Placement2D", "P", (DeriveFn)&calc_IfcAxis2Placement2D_P);
    register_derived("IFC4X1", "IfcAxis2Placement3D", "P", (DeriveFn)&calc_IfcAxis2Placement3D_P);
    register_derived("IFC4X1", "IfcBSplineCurve", "UpperIndexOnControlPoints", (DeriveFn)&calc_IfcBSplineCurve_UpperIndexOnControlPoints);
    register_derived("IFC4X1", "IfcBSplineCurve", "ControlPoints", (DeriveFn)&calc_IfcBSplineCurve_ControlPoints);
    register_derived("IFC4X1", "IfcBSplineCurveWithKnots", "UpperIndexOnKnots", (DeriveFn)&calc_IfcBSplineCurveWithKnots_UpperIndexOnKnots);
    register_derived("IFC4X1", "IfcBSplineSurface", "UUpper", (DeriveFn)&calc_IfcBSplineSurface_UUpper);
    register_derived("IFC4X1", "IfcBSplineSurface", "VUpper", (DeriveFn)&calc_IfcBSplineSurface_VUpper);
    register_derived("IFC4X1", "IfcBSplineSurface", "ControlPoints", (DeriveFn)&calc_IfcBSplineSurface_ControlPoints);
    register_derived("IFC4X1", "IfcBSplineSurfaceWithKnots", "KnotVUpper", (DeriveFn)&calc_IfcBSplineSurfaceWithKnots_KnotVUpper);
    register_derived("IFC4X1", "IfcBSplineSurfaceWithKnots", "KnotUUpper", (DeriveFn)&calc_IfcBSplineSurfaceWithKnots_KnotUUpper);
    register_derived("IFC4X1", "IfcBooleanResult", "Dim", (DeriveFn)&calc_IfcBooleanResult_Dim);
    register_derived("IFC4X1", "IfcBoundingBox", "Dim", (DeriveFn)&calc_IfcBoundingBox_Dim);
    register_derived("IFC4X1", "IfcCartesianPoint", "Dim", (DeriveFn)&calc_IfcCartesianPoint_Dim);
    register_derived("IFC4X1", "IfcCartesianPointList", "Dim", (DeriveFn)&calc_IfcCartesianPointList_Dim);
    register_derived("IFC4X1", "IfcCartesianTransformationOperator", "Scl", (DeriveFn)&calc_IfcCartesianTransformationOperator_Scl);
    register_derived("IFC4X1", "IfcCartesianTransformationOperator", "Dim", (DeriveFn)&calc_IfcCartesianTransformationOperator_Dim);
    register_derived("IFC4X1", "IfcCartesianTransformationOperator2D", "U", (DeriveFn)&calc_IfcCartesianTransformationOperator2D_U);
    register_derived("IFC4X1", "IfcCartesianTransformationOperator2DnonUniform", "Scl2", (DeriveFn)&calc_IfcCartesianTransformationOperator2DnonUniform_Scl2);
    register_derived("IFC4X1", "IfcCartesianTransformationOperator3D", "U", (DeriveFn)&calc_IfcCartesianTransformationOperator3D_U);
    register_derived("IFC4X1", "IfcCartesianTransformationOperator3DnonUniform", "Scl2", (DeriveFn)&calc_IfcCartesianTransformationOperator3DnonUniform_Scl2);
    register_derived("IFC4X1", "IfcCartesianTransformationOperator3DnonUniform", "Scl3", (DeriveFn)&calc_IfcCartesianTransformationOperator3DnonUniform_Scl3);
    register_derived("IFC4X1", "IfcCompositeCurve", "NSegments", (DeriveFn)&calc_IfcCompositeCurve_NSegments);
    register_derived("IFC4X1", "IfcCompositeCurve", "ClosedCurve", (DeriveFn)&calc_IfcCompositeCurve_ClosedCurve);
    register_derived("IFC4X1", "IfcCompositeCurveOnSurface", "BasisSurface", (DeriveFn)&calc_IfcCompositeCurveOnSurface_BasisSurface);
    register_derived("IFC4X1", "IfcCompositeCurveSegment", "Dim", (DeriveFn)&calc_IfcCompositeCurveSegment_Dim);
    register_derived("IFC4X1", "IfcCsgPrimitive3D", "Dim", (DeriveFn)&calc_IfcCsgPrimitive3D_Dim);
    register_derived("IFC4X1", "IfcCurve", "Dim", (DeriveFn)&calc_IfcCurve_Dim);
    register_derived("IFC4X1", "IfcDerivedUnit", "Dimensions", (DeriveFn)&calc_IfcDerivedUnit_Dimensions);
    register_derived("IFC4X1", "IfcDirection", "Dim", (DeriveFn)&calc_IfcDirection_Dim);
    register_derived("IFC4X1", "IfcEdgeLoop", "Ne", (DeriveFn)&calc_IfcEdgeLoop_Ne);
    register_derived("IFC4X1", "IfcFaceBasedSurfaceModel", "Dim", (DeriveFn)&calc_IfcFaceBasedSurfaceModel_Dim);
    register_derived("IFC4X1", "IfcGeometricRepresentationSubContext", "WorldCoordinateSystem", (DeriveFn)&calc_IfcGeometricRepresentationSubContext_WorldCoordinateSystem);
    register_derived("IFC4X1", "IfcGeometricRepresentationSubContext", "CoordinateSpaceDimension", (DeriveFn)&calc_IfcGeometricRepresentationSubContext_CoordinateSpaceDimension);
    register_derived("IFC4X1", "IfcGeometricRepresentationSubContext", "TrueNorth", (DeriveFn)&calc_IfcGeometricRepresentationSubContext_TrueNorth);
    register_derived("IFC4X1", "IfcGeometricRepresentationSubContext", "Precision", (DeriveFn)&calc_IfcGeometricRepresentationSubContext_Precision);
    register_derived("IFC4X1", "IfcGeometricSet", "Dim", (DeriveFn)&calc_IfcGeometricSet_Dim);
    register_derived("IFC4X1", "IfcHalfSpaceSolid", "Dim", (DeriveFn)&calc_IfcHalfSpaceSolid_Dim);
    register_derived("IFC4X1", "IfcMaterialLayerSet", "TotalThickness", (DeriveFn)&calc_IfcMaterialLayerSet_TotalThickness);
    register_derived("IFC4X1", "IfcMirroredProfileDef", "Operator", (DeriveFn)&calc_IfcMirroredProfileDef_Operator);
    register_derived("IFC4X1", "IfcOrientedEdge", "EdgeStart", (DeriveFn)&calc_IfcOrientedEdge_EdgeStart);
    register_derived("IFC4X1", "IfcOrientedEdge", "EdgeEnd", (DeriveFn)&calc_IfcOrientedEdge_EdgeEnd);
    register_derived("IFC4X1", "IfcPlacement", "Dim", (DeriveFn)&calc_IfcPlacement_Dim);
    register_derived("IFC4X1", "IfcPointOnCurve", "Dim", (DeriveFn)&calc_IfcPointOnCurve_Dim);
    register_derived("IFC4X1", "IfcPointOnSurface", "Dim", (DeriveFn)&calc_IfcPointOnSurface_Dim);
    register_derived("IFC4X1", "IfcRationalBSplineCurveWithKnots", "Weights", (DeriveFn)&calc_IfcRationalBSplineCurveWithKnots_Weights);
    register_derived("IFC4X1", "IfcRationalBSplineSurfaceWithKnots", "Weights", (DeriveFn)&calc_IfcRationalBSplineSurfaceWithKnots_Weights);
    register_derived("IFC4X1", "IfcRevolvedAreaSolid", "AxisLine", (DeriveFn)&calc_IfcRevolvedAreaSolid_AxisLine);
    register_derived("IFC4X1", "IfcSIUnit", "Dimensions", (DeriveFn)&calc_IfcSIUnit_Dimensions);
    register_derived("IFC4X1", "IfcSectionedSpine", "Dim", (DeriveFn)&calc_IfcSectionedSpine_Dim);
    register_derived("IFC4X1", "IfcShellBasedSurfaceModel", "Dim", (DeriveFn)&calc_IfcShellBasedSurfaceModel_Dim);
    register_derived("IFC4X1", "IfcSolidModel", "Dim", (DeriveFn)&calc_IfcSolidModel_Dim);
    register_derived("IFC4X1", "IfcSurface", "Dim", (DeriveFn)&calc_IfcSurface_Dim);
    register_derived("IFC4X1", "IfcSurfaceCurve", "BasisSurface", (DeriveFn)&calc_IfcSurfaceCurve_BasisSurface);
    register_derived("IFC4X1", "IfcSurfaceOfLinearExtrusion", "ExtrusionAxis", (DeriveFn)&calc_IfcSurfaceOfLinearExtrusion_ExtrusionAxis);
    register_derived("IFC4X1", "IfcSurfaceOfRevolution", "AxisLine", (DeriveFn)&calc_IfcSurfaceOfRevolution_AxisLine);
    register_derived("IFC4X1", "IfcTable", "NumberOfCellsInRow", (DeriveFn)&calc_IfcTable_NumberOfCellsInRow);
    register_derived("IFC4X1", "IfcTable", "NumberOfHeadings", (DeriveFn)&calc_IfcTable_NumberOfHeadings);
    register_derived("IFC4X1", "IfcTable", "NumberOfDataRows", (DeriveFn)&calc_IfcTable_NumberOfDataRows);
    register_derived("IFC4X1", "IfcTessellatedFaceSet", "Dim", (DeriveFn)&calc_IfcTessellatedFaceSet_Dim);
    register_derived("IFC4X1", "IfcTriangulatedFaceSet", "NumberOfTriangles", (DeriveFn)&calc_IfcTriangulatedFaceSet_NumberOfTriangles);
    register_derived("IFC4X1", "IfcVector", "Dim", (DeriveFn)&calc_IfcVector_Dim);
} }; static __derived_register_t __derived_register_instance;}

}}}  // namespace ifcapi::express::IFC4X1
