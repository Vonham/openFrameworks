#include "ofShape.h"
#include "ofGraphics.h"
#include "ofTessellator.h"

#include <deque>

ofPath::ofPath(){
	bClosed = false;
	bHasChanged = true;
}
const vector<ofPath::Command> & ofPath::getCommands() const{
	return commands;
}

vector<ofPath::Command> & ofPath::getCommands(){
	return commands;
}

void ofPath::addCommand(const ofPath::Command & command){
	commands.push_back(command);
	bHasChanged=true;
}

void ofPath::close(){
	bClosed = true;
}

bool ofPath::isClosed(){
	return bClosed;
}


int ofPath::size(){
	return commands.size();
}

ofPath::Command::Command(Type type , const ofPoint & p)
:type(type)
,to(p)
{}

/// for bezierTo
ofPath::Command::Command(Type type , const ofPoint & p, const ofPoint & cp1, const ofPoint & cp2)
:type(type)
,to(p)
,cp1(cp1)
,cp2(cp2)
{
}

///for arc
ofPath::Command::Command(Type type , const ofPoint & centre, float radiusX, float radiusY, float angleBegin, float angleEnd)
:type(type)
,to(centre)
,radiusX(radiusX)
,radiusY(radiusY)
,angleBegin(angleBegin)
,angleEnd(angleEnd)
{
}

ofShape::ofShape(){
	strokeWidth = 0;
	bFill = true;
	windingMode = OF_POLY_WINDING_ODD;
	prevCurveRes = 16;
	curveResolution = 16;
	arcResolution = 20;
	mode = PATHS;
	bNeedsTessellation = false;
	hasChanged = false;
	bUseShapeColor = false;
	clear();
}

void ofShape::clear(){
	if(mode==PATHS){
		paths.clear();
		hasChanged = true;
	}else{
		polylines.resize(1);
		polylines[0].clear();
		bNeedsTessellation = true;
	}
}

void ofShape::newPath(){
	if(mode==PATHS){
		paths.push_back(ofPath());
	}else{
		polylines.push_back(ofPolyline());
	}
}

void ofShape::lineTo(const ofPoint & p){
	if(mode==PATHS){
		lastPath().addCommand(ofPath::Command(ofPath::Command::lineTo,p));
		hasChanged = true;
	}else{
		lastPolyline().lineTo(p);
		bNeedsTessellation = true;
	}
}

void ofShape::lineTo(float x, float y, float z){
	lineTo(ofPoint(x,y,z));
}

void ofShape::lineTo(float x, float y){
	lineTo(ofPoint(x,y,0));
}

void ofShape::moveTo(const ofPoint & p){
	if(mode==PATHS){
		if(lastPath().size()>0) newPath();
		lastPath().addCommand(ofPath::Command(ofPath::Command::lineTo,p));
		hasChanged = true;
	}else{
		if(lastPolyline().size()>0) newPath();
		lastPolyline().addVertex(p);
		bNeedsTessellation = true;
	}
}

void ofShape::moveTo(float x, float y, float z){
	moveTo(ofPoint(x,y,z));
}

void ofShape::curveTo(const ofPoint & p){
	if(mode==PATHS){
		lastPath().addCommand(ofPath::Command(ofPath::Command::curveTo,p));
		hasChanged = true;
	}else{
		lastPolyline().curveTo(p);
		bNeedsTessellation = true;
	}
}

void ofShape::curveTo(float x, float y, float z){
	curveTo(ofPoint(x,y,z));
}

void ofShape::curveTo(float x, float y){
	curveTo(ofPoint(x,y,0));
}

void ofShape::bezierTo(const ofPoint & cp1, const ofPoint & cp2, const ofPoint & p){
	if(mode==PATHS){
		lastPath().addCommand(ofPath::Command(ofPath::Command::bezierTo,p,cp1,cp2));
		hasChanged = true;
	}else{
		lastPolyline().bezierTo(cp1,cp2,p);
		bNeedsTessellation = true;
	}
}

void ofShape::bezierTo(float cx1, float cy1, float cx2, float cy2, float x, float y){
	bezierTo(ofPoint(cx1,cy1,0),ofPoint(cx2,cy2,0),ofPoint(x,y,0));
}

void ofShape::bezierTo(float cx1, float cy1, float cz1, float cx2, float cy2, float cz2, float x, float y, float z){
	bezierTo(ofPoint(cx1,cy1,cz1),ofPoint(cx2,cy2,cz2),ofPoint(x,y,z));
}

void ofShape::quadBezierTo(const ofPoint & cp1, const ofPoint & cp2, const ofPoint & p){
	if(mode==PATHS){
		lastPath().addCommand(ofPath::Command(ofPath::Command::quadBezierTo,p,cp1,cp2));
		hasChanged = true;
	}else{
		lastPolyline().quadBezierTo(cp1,cp2,p);
		bNeedsTessellation = true;
	}
}

void ofShape::quadBezierTo(float cx1, float cy1, float cx2, float cy2, float x, float y){
	quadBezierTo(ofPoint(cx1,cy1,0),ofPoint(cx2,cy2,0),ofPoint(x,y,0));
}

void ofShape::quadBezierTo(float cx1, float cy1, float cz1, float cx2, float cy2, float cz2, float x, float y, float z){
	quadBezierTo(ofPoint(cx1,cy1,cz1),ofPoint(cx2,cy2,cz2),ofPoint(x,y,z));
}

void ofShape::arc(const ofPoint & centre, float radiusX, float radiusY, float angleBegin, float angleEnd){
	if(mode==PATHS){
		lastPath().addCommand(ofPath::Command(ofPath::Command::arc,centre,radiusX,radiusY,angleBegin,angleEnd));
		hasChanged = true;
	}else{
		lastPolyline().arc(centre,radiusX,radiusY,angleBegin,angleEnd,arcResolution);
		bNeedsTessellation = true;
	}
}

void ofShape::arc(float x, float y, float radiusX, float radiusY, float angleBegin, float angleEnd){
	arc(ofPoint(x,y,0),radiusX,radiusY,angleBegin,angleEnd);
}

void ofShape::arc(float x, float y, float z, float radiusX, float radiusY, float angleBegin, float angleEnd){
	arc(ofPoint(x,y,z),radiusX,radiusY,angleBegin,angleEnd);
}

void ofShape::close(){
	if(mode==PATHS){
		lastPath().close();
		hasChanged = true;
	}else{
		lastPolyline().setClosed(true);
		bNeedsTessellation = true;
	}
	//newPath();
}

void ofShape::setPolyWindingMode(ofPolyWindingMode newMode){
	if(windingMode != newMode){
		windingMode = newMode;
		if(mode==PATHS){
			hasChanged = true;
		}else{
			bNeedsTessellation = true;
		}
	}
}

void ofShape::setFilled(bool hasFill){
	if(bFill != hasFill){
		bFill = hasFill;
		if(bFill) strokeWidth = 0;
		else if(strokeWidth==0) strokeWidth = 1;
		if(mode==PATHS){
			hasChanged = true;
		}else{
			bNeedsTessellation = true;
		}
	}
}

void ofShape::setStrokeWidth(float width){
	if(width != 0 && strokeWidth == 0){
		if(mode==PATHS){
			hasChanged = true;
		}else{
			bNeedsTessellation = true;
		}
	}
	strokeWidth = width;
}

ofPath & ofShape::lastPath(){
	if(paths.empty()){
		paths.push_back(ofPath());
	}
	return paths.back();
}

ofPolyline & ofShape::lastPolyline(){
	if(polylines.empty()){
		polylines.push_back(ofPolyline());
	}
	return polylines.back();
}

vector<ofPath> & ofShape::getPaths(){
	if(mode==POLYLINES){
		ofLog(OF_LOG_WARNING,"trying to get paths from shape with polylines only");
	}
	return paths;
}

const vector<ofPath> & ofShape::getPaths() const{
	if(mode==POLYLINES){
		ofLog(OF_LOG_WARNING,"trying to get paths from shape with polylines only");
	}
	return paths;
}


ofPolyWindingMode ofShape::getWindingMode() const{
	return windingMode;
}

bool ofShape::isFilled() const{
	return bFill;
}

ofColor ofShape::getFillColor() const{
	return fillColor;
}

ofColor ofShape::getStrokeColor() const{
	return strokeColor;
}

float ofShape::getStrokeWidth() const{
	return strokeWidth;
}

void ofShape::generatePolylinesFromPaths(){
	if(mode==POLYLINES) return;
	if(hasChanged || curveResolution!=prevCurveRes){
		prevCurveRes = curveResolution;

		polylines.clear();
		polylines.resize(paths.size());
		for(int j=0;j<(int)paths.size();j++){
			const vector<ofPath::Command> & commands = paths[j].getCommands();

			for(int i=0; i<(int)commands.size();i++){
				switch(commands[i].type){

				case ofPath::Command::lineTo:
					polylines[j].addVertex(commands[i].to);
					break;
				case ofPath::Command::curveTo:
					polylines[j].curveTo(commands[i].to, curveResolution);
					break;
				case ofPath::Command::bezierTo:
					polylines[j].bezierTo(commands[i].cp1,commands[i].cp2,commands[i].to, curveResolution);
					break;
				case ofPath::Command::quadBezierTo:
					polylines[j].quadBezierTo(commands[i].cp1,commands[i].cp2,commands[i].to, curveResolution);
					break;
				case ofPath::Command::arc:
					polylines[j].arc(commands[i].to,commands[i].radiusX,commands[i].radiusY,commands[i].angleBegin,commands[i].angleEnd, arcResolution);
					break;
				}
			}
			polylines[j].setClosed(paths[j].isClosed());
		}
		hasChanged = false;
		bNeedsTessellation = true;
	}
}

void ofShape::tessellate(){
	generatePolylinesFromPaths();
	if(!bNeedsTessellation) return;
	//bool bIs2D = !bIs3D;
	if(bFill){
		ofTessellator::tessellateToCache( polylines, windingMode, cachedTessellation);
	}
	if ( hasOutline() ){
		if( windingMode != OF_POLY_WINDING_ODD ) {
			ofTessellator::tessellateToOutline( polylines, windingMode, tessellatedPolylines);
		}
	}
	bNeedsTessellation = false;
}

vector<ofPolyline> & ofShape::getOutline() {
	tessellate();
	if( windingMode != OF_POLY_WINDING_ODD ) {
		return tessellatedPolylines;
	}else{
		return polylines;
	}
}

vector<ofPrimitive> & ofShape::getTessellation(){
	tessellate();
	cachedTessellation.meshes.resize(cachedTessellation.numElements);
	return cachedTessellation.meshes;
}

void ofShape::updateShape(){
	generatePolylinesFromPaths();
}

void ofShape::draw(float x, float y){
	ofPushMatrix();
	ofTranslate(x,y);
	draw();
	ofPopMatrix();
}


void ofShape::draw(){
	tessellate();
	if(bFill){
		if(bUseShapeColor){
			ofPushStyle();
			ofSetColor(fillColor);
		}
		for(int i=0;i<cachedTessellation.numElements && i<cachedTessellation.meshes.size();i++){
			ofGetDefaultRenderer()->draw(cachedTessellation.meshes[i]);
		}
		if(bUseShapeColor){
			ofPopStyle();
		}
	}

	if(hasOutline()){
		if(bUseShapeColor){
			ofPushStyle();
			ofSetColor(fillColor);
		}
		vector<ofPolyline> & polys = getOutline();
		for(int i=0;i<polys.size();i++){
			ofGetDefaultRenderer()->draw(polys[i]);
		}
		if(bUseShapeColor){
			ofPopStyle();
		}
	}
}


void ofShape::markedChanged(){
	if(mode==PATHS){
		hasChanged = true;
	}else{
		bNeedsTessellation = true;
	}
}


void ofShape::setMode(Mode _mode){
	mode = _mode;
}

void ofShape::setCurveResolution(int _curveResolution){
	curveResolution = _curveResolution;
}

int ofShape::getCurveResolution(){
	return curveResolution;
}

void ofShape::setArcResolution(int res){
	arcResolution = res;
}

int ofShape::getArcResolution(){
	return arcResolution;
}

void ofShape::setUseShapeColor(bool useColor){
	bUseShapeColor = useColor;
}

bool ofShape::getUseShapeColor(){
	return bUseShapeColor;
}

void ofShape::setColor( const ofColor& color ) {
	setFillColor( color );
	setStrokeColor( color );
}

void ofShape::setHexColor( int hex ) {
	setColor( ofColor().fromHex( hex ) );
}

void ofShape::setFillColor(const ofColor & color){
	setUseShapeColor(true);
	fillColor = color;
}

void ofShape::setFillHexColor( int hex ) {
	setFillColor( ofColor().fromHex( hex ) );
}

void ofShape::setStrokeColor(const ofColor & color){
	setUseShapeColor(true);
	strokeColor = color;
}

void ofShape::setStrokeHexColor( int hex ) {
	setStrokeColor( ofColor().fromHex( hex ) );
};