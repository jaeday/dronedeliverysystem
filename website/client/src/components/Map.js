import React, { useRef, useEffect, useState } from 'react';
import mapboxgl from 'mapbox-gl';
import './Map.css';
import geoJson from './MapMarkers.json';
import { PureComponent } from 'react';
import ReactMapGL, { Popup } from 'react-map-gl';
import { Container, Col, Row, Button } from 'reactstrap';




const mapStyle = {
    width: '100%',
    height: 600
}

const mapboxApiKey = 'pk.eyJ1IjoiamFlZGF5IiwiYSI6ImNrc2ljMTNyeTA0c3gzMnAwcHgyaGViaWgifQ.zurLLuNDk452jDGmEAA8mg';

const delivery = {
    location: ""
}

async function locationSelect(locationInput){
    await localStorage.setItem('location', locationInput);
}



class MapView extends PureComponent {

    constructor(props) {
        super(props);
        this.state = {
            viewport: {
                latitude: 42.2772,
                longitude: -83.7381,
                zoom: 16
            },
            tempMarker: null,
            markers: []
        };

    }

    

    render() {
        const { viewport, tempMarker, markers } = this.state;
        return (
            <Container fluid={true} style={{ height: '600px', width: '1200px', marginLeft: '30px' }}>
                <Row>
                    <Col>
                        <ReactMapGL
                            mapboxApiAccessToken={mapboxApiKey}
                            mapStyle="mapbox://styles/mapbox/streets-v11"
                            {...viewport}
                            {...mapStyle}
                            onViewportChange={(viewport) => this.setState({ viewport })}
                        >

                            {
                                geoJson.features.map((feature) => {
                                    return (
                                        <Popup
                                            closeButton={true}
                                            closeOnClick={false}
                                            longitude={feature.geometry.coordinates[1]}
                                            latitude={feature.geometry.coordinates[0]}>
                                            <h3 style={{ fontSize: '10px' }}>{feature.properties.description}</h3>
                                            <button className="btn-small" onClick={() => locationSelect(feature.properties.title)}>Order Here</button>
                                        </Popup>
                                    )
                                })
                            }
                        </ReactMapGL>
                    </Col>
                </Row>
            </Container>
        );
    }
}

export default MapView;



