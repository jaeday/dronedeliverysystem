import React, { useState, useEffect } from 'react'
import { useDispatch, useSelector } from 'react-redux'
import {filterCoffees} from '../actions/coffeeActions'

export default function Filter() {
    const dispatch = useDispatch()
    const[searchkey, setsearchkey] = useState()
    const[category, setcategory] = useState('all')
    return(
        <div className='filterContainer center-align'>
            <div className ="row shadow-lg p-3 mb-5 bg-white rounded">
                <div className = "col-sm">
                    <input 
                    onChange={(e)=>{setsearchkey(e.target.value)}}
                    value ={searchkey}type = "text" className="form-control w-100" placeholder="search coffees"/>
                </div>
                <div className = "col-sm">
                    <select className="form-control w-100 mt-2" value={category} onChange={(e)=>setcategory(e.target.value)}>
                        <option value ="all">All</option>
                        <option value ="nondairy">Non Dairy</option>
                        <option value ="dairy">Dairy</option>
                    </select>

                </div>
                <div className = "col-sm">
                    <button className = "btn w-100 mt-2" onClick={()=>{dispatch(filterCoffees(searchkey, category))}}> Filter</button>
                </div>
            </div>
        </div>
    )
}