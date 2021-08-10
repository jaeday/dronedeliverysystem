import React from 'react'
import {useSelector, useDispatch } from 'react-redux'
import { addToCart } from '../actions/cartActions'

export default function Navbar() {
    const cartstate = useSelector(state=>state.cartReducer)
    return (
        <div>
            <nav className="navbar navbar-expand-lg shadow-lg p-3 mb-5 bg-white rounded">
                <a className="navbar-brand" href="/">Jaes' Coffee</a>
                <button className="navbar-toggler" type="button" data-toggle="collapse" data-target="#navbarNavAltMarkup" aria-controls="navbarNavAltMarkup" aria-expanded="false" aria-label="Toggle navigation">
                    <span className="navbar-toggler-icon"></span>
                </button>
                <div className="collapse navbar-collapse" id="navbarNavAltMarkup">
                    <div className="navbar-nav ms-auto">
                        <a className="nav-item nav-link" href="#">Login <span className="sr-only"></span></a>
                        <a className="nav-item nav-link" href="/cart">Cart {cartstate.cartItems.length}</a>
                    </div>
                </div>
            </nav>
        </div>
    )
}