import React from 'react'
import { useSelector, useDispatch } from 'react-redux'
import { addToCart } from '../actions/cartActions'
import { deleteFromCart } from '../actions/cartActions'
import Checkout from '../components/Checkout'
import Map from '../components/Map'
import MapGl from 'mapbox-gl';
import '../index.css'

export default function Cartscreen() {

    const cartstate = useSelector(state => state.cartReducer)
    const cartItems = cartstate.cartItems
    const dispatch = useDispatch()
    var subtotal = cartItems.reduce((x, item) => x + item.price, 0)



    return (
        <div className = "container center">
            <h2>Select delivery location:</h2>
                <div className="row " style={{height:'600px', width: '1200px', marginLeft:'30px' }}>
                    <Map />
                </div>
            <div className="row" style={{paddingTop: '30px'}}>
                <div className="col">
                    <h2 style={{ fontSize: '40px' , paddingTop: '10px'}}>My Cart</h2>
                    {cartItems.map(item => {
                        return <div className="flex-container center">
                            <div className='text-left w-100 ml-1'>
                                <h1>{item.name} [{item.size}]</h1>
                                {console.log(item.quantity)}
                                <h1>Price : {item.quantity} * {item.prices[0][item.size]} = {item.price}</h1>
                                <h1 style={{ display: 'inline' }}>Quantity : </h1>
                                <i className="fa fa-plus" aria-hidden="true" onClick={() => { dispatch(addToCart(item, item.quantity + 1, item.size)) }}></i>
                                <b>{item.quantity}</b>
                                <i className="fa fa-minus" aria-hidden="true" onClick={() => { dispatch(addToCart(item, item.quantity - 1, item.size)) }}></i>
                                <hr />
                            </div>
                            <div className='m-1 w-100'>
                                {console.log(item)}
                                <img src={item.image} style={{ height: '80px', width: '80px' }} />
                            </div>
                            <div className='m-1 w-100'>
                                <i className="fa fa-trash mt-5" aria-hidden="true" onClick={() => { dispatch(deleteFromCart(item, item.quantity - 1, item.size)) }}></i>
                            </div>
                        </div>
                    })}

                    {cartItems.length >= 6 && <div className="flex-container">
                        <h1>You are ordering too many coffees at once. As per company policy, we are trying to reduce the delivery time for other students by
                            limiting the amount of coffees each can order. Please take off some of the coffee and try again.
                        </h1>
                    </div>}

            

                </div>
                
            </div>
                
                <div className="row center" style={{height:'200px', width: '1200px', marginLeft: '48px'}}>
                    <h2 style={{ fontSize: '45px' ,paddingTop: '50px'}}>SubTotal: ${subtotal}</h2>
                    <Checkout subtotal={subtotal} />
                </div>
            

        </div>

    )
}