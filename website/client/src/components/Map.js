import React, { useRef, useEffect, useState } from 'react';
import mapboxgl from 'mapbox-gl';
import './Map.css';
import geoJson from './MapMarkers.json';


mapboxgl.accessToken =
    'pk.eyJ1IjoiamFlZGF5IiwiYSI6ImNrc2ljMTNyeTA0c3gzMnAwcHgyaGViaWgifQ.zurLLuNDk452jDGmEAA8mg';

const Map = () => {
    const mapContainerRef = useRef(null);

    const [lng, setLng] = useState(-83.7381);
    const [lat, setLat] = useState(42.2772);
    const [zoom, setZoom] = useState(16.27);


    // Initialize map when component mounts
    useEffect(() => {
        const map = new mapboxgl.Map({
            container: mapContainerRef.current,
            style: 'mapbox://styles/mapbox/streets-v11',
            center: [lng, lat],
            width: 0,
            height: 0,
            latitude: 42.2772,
            longitude: -83.7381,
            zoom: 16,
            attributionControl: false
        });

        // Add navigation control (the +/- zoom buttons)
        map.addControl(new mapboxgl.NavigationControl(), 'top-right');

        map.on('move', () => {
            setLng(map.getCenter().lng.toFixed(4));
            setLat(map.getCenter().lat.toFixed(4));
            setZoom(map.getZoom().toFixed(2));
        });

        geoJson.features.map((feature) =>
            new mapboxgl.Marker().setLngLat(feature.geometry.coordinates).addTo(map)
        );

        // Clean up on unmount
        return () => map.remove();
    }, []); // eslint-disable-line react-hooks/exhaustive-deps

    return (
        <div className="col mapbox-logo" ref={mapContainerRef} />
    );
};

export default Map;